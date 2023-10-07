#include <stdio.h>

#include "disassembler.h"
#include "../common/commands.h"
#include "../common/log_funcs.h"
#include "../common/errors.h"

static const char* DEFAULT_OUT = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/asm_code.txt";
static const char* DEFAULT_IN  = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.txt";

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {};

    FILE* in_stream  = nullptr;
    FILE* out_stream = nullptr;

    const char* input_file  = nullptr;
    const char* output_file = nullptr;

    if (argc < 2)
        input_file = DEFAULT_IN;
    else
        input_file = argv[1];

    if (argc < 3)
        output_file = DEFAULT_OUT;
    else
        output_file = argv[2];

    in_stream  = fopen(input_file, "r");
    out_stream = fopen(output_file, "w");

    if (in_stream == nullptr)
    {
        error.code = ERRORS::OPEN_FILE;
        error.data = (void*) input_file;
        EXIT_IF_ERROR(&error);
    }

    if (out_stream == nullptr)
    {
        error.code = ERRORS::OPEN_FILE;
        error.data = (void*) output_file;
        EXIT_IF_ERROR(&error);
    }

    CommandErrors asm_err = HandleCode(in_stream, out_stream);
    if (asm_err != CommandErrors::OK)
    {
        error.code = ERRORS::ASM_ERROR;
        EXIT_IF_ERROR(&error);
    }

    fclose(in_stream);
    fclose(out_stream);
}
