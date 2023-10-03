#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <string.h>

#include "oplist.h"
#include "platforms/nasm_x86_64_linux.h"

const char* bfOpCodes = "<>,.+-[]";

/**
* @return true if the code is valid
*/
bool validateCode(const char* code, size_t length) {
    // just make sure brackets match
    long openBrackets = 0;
    
    size_t currentLine = 1;
    size_t currentChar = 1;

    for(size_t i = 0; i < length; i++) {
        if (code[i] == '[') {
            openBrackets++;
        } else if (code[i] == ']') {
            openBrackets--;
        }

        if (openBrackets < 0) {
            fprintf(stderr, "mismatching brackets on %zu:%zu\n", currentLine, currentChar);
            return false;
        }

        if (code[i] == '\n') {
            currentLine++;
            currentChar = 1;
        } else {
            currentChar++;
        }
    }

    if (openBrackets > 0) {
        fprintf(stderr, "unclosed brackets!\n");
        return false;
    }
    
    return true;
}

char* stripCode(const char* code, size_t length) {
    size_t codeLen = 0;
    for (size_t i = 0; i < length; i++) {
        if (strchr(bfOpCodes, code[i]) != NULL) {
            codeLen++;
        }
    }

    if (codeLen == 0) {
        fprintf(stderr, "No code found!\n");
        return NULL;
    }

    char* condensedCode = calloc(codeLen + 1, 1);

    size_t condensedPos = 0;
    for (size_t i = 0; i < length; i++) {
        if (strchr(bfOpCodes, code[i]) != NULL) {
            condensedCode[condensedPos] = code[i];

            condensedPos++;
        }
    }

    return condensedCode;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "please specify BF file to compile!\n");
        return EXIT_FAILURE;
    }

    FILE* bfFile = fopen(argv[1], "r");
    if(!bfFile) {
        fprintf(stderr, "failed to open file!\n");
        return EXIT_FAILURE;
    }

    if (fseek(bfFile, 0, SEEK_END) == -1) {
        fprintf(stderr, "failed to seek to end!\n");
        return EXIT_FAILURE;
    }

    long bfFileLength = ftell(bfFile);

    if (fseek(bfFile, 0, SEEK_SET)) {
        fprintf(stderr, "failed to rewind to beginning!\n");
        return EXIT_FAILURE;
    }


    char* bfFileContents = calloc(bfFileLength + 1, 1);

    if (!fread(bfFileContents, 1, bfFileLength, bfFile)) {
        fprintf(stderr, "failed to read!\n");
        return EXIT_FAILURE;
    } 

    if (!validateCode(bfFileContents, bfFileLength)) {
        fprintf(stderr, "Code is invalid!\n");
        return EXIT_FAILURE;
    }

    char* condensedContents = stripCode(bfFileContents, bfFileLength);
    size_t condensedContentsLength = strlen(condensedContents);

    OpBlock_t* ops = generate_ops_from_code(condensedContents, condensedContentsLength);

    // print_ops(stdout, ops);

    FILE* assemblyFile = fopen("out.S", "w");
    gen_nasm_x86_64_linux(ops, assemblyFile);

    free_ops(ops);
    free(condensedContents);
    free(bfFileContents);
    fclose(bfFile);

    return EXIT_SUCCESS;
}
