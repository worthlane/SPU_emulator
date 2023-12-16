#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "../common/logs.h"
#include "../common/types.h"
#include "../common/input_and_output.h"
#include "../common/file_read.h"
#include "../common/hash.h"
#include "spu.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {};

    spu_t spu = {};

    const char* file_name = GetFileName(argc, argv, 1, "BYTE CODE", &error);

    FILE* in_stream = OpenFile(file_name, "rb", &error);

    SPUCtor(&spu, file_name, in_stream, &error);

    EXIT_IF_ERROR(&error);

    RunSPU(&spu);

    if (spu.status != SPUErrors::NONE)
        error.code  = (int) ERRORS::SPU_ERROR;

    EXIT_IF_ERROR(&error);

    SPUDtor(&error, &spu);

    return 0;
}
