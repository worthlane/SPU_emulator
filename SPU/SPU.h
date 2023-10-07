#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "stack.h"
#include "../common/commands.h"

struct SPUInfo
{
    Stack_t     stack;
    const char* file_name;
    FILE*       fp;
    elem_t      registers[REG_AMT];
};

typedef struct SPUInfo spu_t;

static const char* DEFAULT_IN  = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.txt";

int SPUCtor(spu_t* spu_info, const char* file_name = DEFAULT_IN);
int SPUDtor(spu_t* spu_info);
int RunSPU(spu_t* spu_info);


#endif
