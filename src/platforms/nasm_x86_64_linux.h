#ifndef NASM_X86_64_LINUX_H
#define NASM_X86_64_LINUX_H

#include <stdlib.h>
#include <stdbool.h>

#include "../oplist.h"

bool gen_nasm_x86_64_linux(OpBlock_t* ops, FILE* oFile);

#endif