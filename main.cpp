#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "log_funcs.h"
#include "types.h"
#include "hash.h"
#include "SPU.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    Stack_t          stk   = {};
    struct ErrorInfo error = {};

    spu_t spu = {};

    if (argc == 1)
        SPUCtor(&spu);
    else
        SPUCtor(&spu, argv[1]);

    error.code = (ERRORS) RunSPU(&spu);
    EXIT_IF_ERROR(&error);

    SPUDtor(&spu);


    return 0;
}
