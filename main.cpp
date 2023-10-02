#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "log_funcs.h"
#include "types.h"
#include "hash.h"
#include "SPU.h"

int main(const int argc, const char* argv[])
{
    if (argc == 1)
        OpenLogFile(argv[0]);
    else
        OpenLogFile(argv[1]);

    Stack_t          stk   = {};
    struct ErrorInfo error = {};

    spu_t spu = {};

    SPUCtor(&spu);

    RunSPU(&spu);

    SPUDtor(&spu);
    
    return 0;
}
