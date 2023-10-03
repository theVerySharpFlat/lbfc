#include "nasm_x86_64_linux.h"
#include <stdio.h>

static const char* header =
    "global _start\n"
    "_start:\n"
    "    mov rax, 9\n"          // mmap (for memory allocation)
    "    xor rdi, rdi\n"        //    addr, 0 to let ther kernel decide where to create the mapping
    "    mov rsi, 4096\n"       //    length, 4096 bytes 
    "    mov rdx, 3\n"          //    prot, PROT_READ | PROT_WRITE = 0b11
    "    mov r10, 0x20|0x02\n"  //    flags, MAP_PRIVATE|MAP_ANONYMOUS
    "    mov r8, -1\n"          //    fd, -1 for compat
    "    xor r9, r9\n"          //    offset, 0
    "    syscall\n"
    "    cmp rax, -4095\n"
    "    jb .mmap_success\n"   // Jump if the return value isn't -1 (MAP_FAILED), otherwise exit
    "\n"
                                // Exit failure
    "    mov rax, 60\n"
    "    mov rdi, 01\n"
    "    syscall\n"
    ".mmap_success:\n"
    "    mov r8, rax\n"         // Store mapped ptr in r8 for all operations
    "    mov r9, r8\n"          // Store base ptr in r9 for unmap
    ;

static const char* footer =
    "    mov rax, 11\n"        // munmap
    "    mov rdi, r9\n"        // Base adress
    "    mov rsi, 4096\n"      // Alloc size
    "    cmp rax, -4095\n"
    "    jb .munmap_success\n" // Jump if the return value isn't -1 (MAP_FAILED), otherwise exit
    "    mov rax, 60\n"        // Exit
    "    mov rdi, 02\n"        // ExitCode = 2
    "    syscall\n"
    "\n"
    ".munmap_success:\n"
    "    mov rax, 60\n"        // Exit
    "    xor rdi, rdi\n"       // ExitCode = 0
    "    syscall\n";

bool gen_nasm_x86_64_linux(OpBlock_t *ops, FILE *oFile) {
    fprintf(oFile, "%s", header);

    for (OpBlock_t* block = ops->block; block->pNextBlock != NULL; block = block->pNextBlock) {
        for (size_t i = 0; i < OP_BLOCK_SIZE; i++) {
            Op_t op = block->block[i];
            
            switch (op.type) {
                case OP_ADD: {
                    if (op.n >= 0) {
                        fprintf(oFile, 
                            "    add BYTE [r8], BYTE %ld\n", op.n);
                    } else {
                        fprintf(oFile, 
                            "    sub BYTE [r8], BYTE %ld\n", -1 * op.n);
                    }
                } break;
                case OP_PRG_END: {
                    // Nothing???
                } break;
                case OP_OUT: {
                    fprintf(oFile,
                    "    mov rax, 1\n"  // write
                            "    mov rdi, 1\n"  // fileno: stdout
                            "    mov rsi, r8\n" // buf: just the bf mem ptr
                            "    mov rdx, 1\n"  // count. 1 byte
                            "    syscall\n"
                    );
                } break;
                case OP_IN: {
                    fprintf(oFile,
                        "    mov rax, 0\n"   // read
                                "    mov rdi, 0\n"  // stdin
                                "    mov rsi, r8\n" // read to r8
                                "    mov rdx, 1\n"  // read 1 byte
                                "    syscall\n"
                    );
                } break;
                case OP_LBL: {
                    fprintf(oFile,
                        "LABEL_%ld:\n", op.n);
                } break;
                case OP_DP: {
                    if (op.n >= 0) {
                        fprintf(oFile, 
                            "    add r8, %ld\n", op.n);
                    } else {
                        fprintf(oFile, 
                            "    sub r8, %ld\n", -1 * op.n);
                    }
                } break;
                case OP_JNE:
                    fprintf(oFile,
                        "    cmp BYTE [r8], BYTE 0\n"
                                "    jne LABEL_%ld\n", op.n
                        );
                } break;
        }
    }

    fprintf(oFile, "%s", footer);

    return true;
}