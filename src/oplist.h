#ifndef OPLIST_H
#define OPLIST_H

#include <stdlib.h>
#include <stdio.h>


#define OP_BLOCK_SIZE 1 // N ops per block

typedef enum {
    OP_PRG_END = 0,
    OP_ADD,
    OP_OUT,
    OP_IN,
    OP_LBL,
    OP_DP,
    OP_JNE
} OP_TYPE;

typedef struct {
    OP_TYPE type;
    long n;
} Op_t;

typedef struct {
    Op_t block[OP_BLOCK_SIZE];

    struct OpBlock_t* pNextBlock;
} OpBlock_t;

OpBlock_t* generate_ops_from_code(const char* code, size_t length);

void print_ops(FILE* oFile, OpBlock_t* blockRoot);

void free_ops(OpBlock_t* root);

#endif