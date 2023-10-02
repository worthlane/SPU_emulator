#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "stack.h"

struct SPUInfo
{
    Stack_t*    stack;
    const char* file_name;
    FILE*       fp;
};

typedef struct SPUInfo spu_t;

static const char* DEFAULT_FILE = "stdin";

int SPUCtor(spu_t* spu_info, Stack_t* stk, const char* file_name = DEFAULT_FILE);
int SPUDtor(spu_t* spu_info);
int RunSPU(spu_t* spu_info);

int Push(spu_t* spu_info);
int Output(spu_t* spu_info);
int Substract(spu_t* spu_info);
int Add(spu_t* spu_info);
int Multiply(spu_t* spu_info);
int Divide(spu_t* spu_info);


#endif
