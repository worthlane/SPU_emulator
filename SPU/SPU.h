#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "../stack/stack.h"

struct SPUInfo
{
    Stack_t     stack;
    const char* file_name;
    FILE*       fp;
};

typedef struct SPUInfo spu_t;

static const char* DEFAULT_IN  = "../assets/machine_code.txt";

int SPUCtor(spu_t* spu_info, const char* file_name = DEFAULT_IN);
int SPUDtor(spu_t* spu_info);
int RunSPU(spu_t* spu_info);


#endif
