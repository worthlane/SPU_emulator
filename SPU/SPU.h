#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "stack.h"
#include "../common/commands.h"
#include "../common/input_and_output.h"

struct SPUInfo
{
    Stack_t     stack;

    const char* file_name;
    FILE*       fp;
    char*       input_buf;
    char*       curr_input_byte;

    elem_t      registers[REG_AMT];
};

static const int SPU_VER = 2;

typedef struct SPUInfo spu_t;

static const char* DEFAULT_IN  = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.txt";

ERRORS SPUCtor(ErrorInfo* error, spu_t* spu_info, const char* file_name = DEFAULT_IN);
ERRORS SPUDtor(ErrorInfo* error, spu_t* spu_info);
ERRORS RunSPU(ErrorInfo* error, spu_t* spu_info);

enum SPUErrors
{
};

#endif
