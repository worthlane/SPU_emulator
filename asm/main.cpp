#include <stdio.h>

#include "assembler.h"
#include "../common/asm_funcs.h"
#include "../common/log_funcs.h"
#include "../common/errors.h"

static const char* DEFAULT_IN      = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/asm_code.txt";
static const char* DEFAULT_OUT     = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.txt";
static const char* DEFAULT_BIN_OUT = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.bin";

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {};

    const char* input_file      = nullptr;
    const char* output_file     = nullptr;
    const char* output_bin_file = nullptr;

    if (argc < 2)
        input_file = DEFAULT_IN;
    else
        input_file = argv[1];

    if (argc < 3)
        output_file = DEFAULT_OUT;
    else
        output_file = argv[2];

    if (argc < 4)
        output_bin_file = DEFAULT_BIN_OUT;
    else
        output_bin_file = argv[3];

    Storage info = {};

    FILE* in_stream  = OpenInputFile(input_file, &info, &error);
    EXIT_IF_ERROR(&error);

    FILE* out_stream = OpenOutputFile(output_file, &error);
    EXIT_IF_ERROR(&error);

    FILE* out_bin_stream = OpenBinOutputFile(output_bin_file, &error);
    EXIT_IF_ERROR(&error);

    AsmErrors asm_err = Assembly(in_stream, out_stream, out_bin_stream, &info);
    if (asm_err != AsmErrors::NONE)
    {
        error.code = ERRORS::ASM_ERROR;
        EXIT_IF_ERROR(&error);
    }

    fclose(in_stream);
    fclose(out_stream);
}
