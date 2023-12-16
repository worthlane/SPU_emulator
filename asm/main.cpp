#include <stdio.h>

#include "assembler.h"
#include "../common/asm_common.h"
#include "../common/logs.h"
#include "../common/errors.h"
#include "../common/file_read.h"

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {};

    const char* input_file      = GetFileName(argc, argv, 1, "INPUT", &error);
    const char* output_file     = GetFileName(argc, argv, 2, "WORD OUTPUT", &error);
    const char* output_bin_file = GetFileName(argc, argv, 3, "BINARY OUTPUT", &error);

    FILE* in_stream      = OpenFile(input_file, "rb", &error);
    EXIT_IF_ERROR(&error);

    FILE* out_stream     = OpenFile(output_file, "w", &error);
    EXIT_IF_ERROR(&error);

    FILE* out_bin_stream = OpenFile(output_bin_file, "wb", &error);
    EXIT_IF_ERROR(&error);

    LinesStorage info = {};
    CreateTextStorage(&info, &error, input_file);

    AsmErrors asm_err = Assembly(out_stream, out_bin_stream, &info); // TODO буффер
    if (asm_err != AsmErrors::NONE)
    {
        error.code = (int) ERRORS::ASM_ERROR;
        EXIT_IF_ERROR(&error);
    }

    fclose(in_stream);
    fclose(out_stream);
}
