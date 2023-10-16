#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "../common/log_funcs.h"
#include "../common/types.h"
#include "../common/hash.h"
#include "SPU.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {ERRORS::NONE};

    spu_t spu = {};

    if (argc == 1)
        SPUCtor(&error, &spu);
    else
        SPUCtor(&error, &spu, argv[1]);

    EXIT_IF_ERROR(&error);

    RunSPU(&spu);

    if (spu.status != SPUErrors::NONE)
        error.code  = ERRORS::SPU_ERROR;

    EXIT_IF_ERROR(&error);

    SPUDtor(&error, &spu);

    return 0;
}
