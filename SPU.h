#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "stack.h"

struct SPUInfo
{
    Stack_t     stack;
    const char* file_name;
    FILE*       code_file;
};

typedef struct SPUInfo spu_t;

int SPUCtor(spu_t* spu_info);
int SPUDtor(spu_t* spu_info);
int RunSPU(spu_t* spu_info);

int Push(spu_t* spu_info);
int Output(spu_t* spu_info);

#endif
