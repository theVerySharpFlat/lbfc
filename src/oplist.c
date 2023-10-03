#include "oplist.h"
#include "bracket_matcher_stack.h"
#include <stdio.h>

OpBlock_t* generate_ops_from_code(const char* code, size_t length) {
    OpBlock_t* blocks_root = calloc(sizeof(OpBlock_t), 1);
    OpBlock_t* current_block = blocks_root;

    size_t currentOpIndex = 0;

    Bracket_Matcher_t* bracket_matcher = allocate_bracket_matcher();

    for (size_t i = 0; i < length; i++) {
        switch (code[i]) {
            case '+':
            case '-': {
                long net = 0;

                size_t j = i;
                for (; j < length; j++) {
                    if (code[j] == '+') {
                        net++;
                    } else if (code[j] == '-') {
                        net--;
                    } else {
                        break;
                    }
                }

                Op_t op = {
                    OP_ADD,
                    net
                };

                if (net != 0) {
                    current_block->block[currentOpIndex] = op;
                    currentOpIndex++;
                }

                i = j - 1;
            } break;

            case '<':
            case '>': {
                long net = 0;

                size_t j = i;
                for (; j < length; j++) {
                    if (code[j] == '>') {
                        net++;
                    } else if (code[j] == '<') {
                        net--;
                    } else {
                        break;
                    }
                }

                Op_t op = {
                    OP_DP,
                    net
                };

                if (net != 0) {
                    current_block->block[currentOpIndex] = op;
                    currentOpIndex++;
                }

                i = j - 1;
            } break;

            case '.': {
                Op_t op = {
                    OP_OUT,
                    0
                };

                current_block->block[currentOpIndex] = op;
                currentOpIndex++;
            } break;

            case ',': {
                Op_t op = {
                    OP_IN,
                    0
                };

                current_block->block[currentOpIndex] = op;
                currentOpIndex++;
            } break;

            case '[': {
                Op_t op = {
                    OP_LBL,
                    bracket_matcher_push_bracket(bracket_matcher)
                };

                size_t j = i;
                for (; j < length; j++) {
                    if (code[j] != '[') {
                        break;
                    }
                }

                i = j - 1;

                current_block->block[currentOpIndex] = op;
                currentOpIndex++;
            } break;

            case ']': {
                Op_t op = {
                    OP_JE,
                    bracket_matcher_pop_bracket(bracket_matcher)
                };

                size_t j = i;
                for (; j < length; j++) {
                    if (code[j] != ']') {
                        break;
                    }
                }

                i = j - 1;

                current_block->block[currentOpIndex] = op;
                currentOpIndex++;
            } break;


            default: {
                fprintf(stderr, "Invalid opchar \'%c\' detected!\n", code[i]);
                free_bracket_matcher(bracket_matcher);
                return NULL;
            }
        }

        if (currentOpIndex >= OP_BLOCK_SIZE) {
            current_block->pNextBlock = calloc(sizeof(OpBlock_t), 1);

            current_block = current_block->pNextBlock;
            
            currentOpIndex = 0;
        }
    }

    free_bracket_matcher(bracket_matcher);

    return blocks_root;
}

void print_ops(FILE* oFile, OpBlock_t* blockRoot) {
    for (size_t i = 0; i < OP_BLOCK_SIZE && blockRoot->block[i].type != OP_PRG_END; i++) {
        Op_t op = blockRoot->block[i];

        switch (op.type) {
            case OP_ADD: fprintf(oFile, "OP_ADD (%ld)\n", op.n); break;
            case OP_DP: fprintf(oFile, "OP_DP (%ld)\n", op.n); break;
            case OP_LBL: fprintf(oFile, "OP_LBL (LBL_%ld)\n", op.n); break;
            case OP_JE: fprintf(oFile, "OP_JE (to LBL_%ld)\n", op.n); break;
            case OP_IN: fprintf(oFile, "OP_IN\n"); break;
            case OP_OUT: fprintf(oFile, "OP_OUT\n"); break;
            default: fprintf(oFile, "Unkown OP (%ld)\n", op.n); break;
        }
    }

    if(blockRoot->pNextBlock != NULL)
        print_ops(oFile, blockRoot->pNextBlock);
}

void free_ops(OpBlock_t* block) {
    if (block->pNextBlock != NULL) {
        free_ops(block->pNextBlock);
    }

    free(block);
}