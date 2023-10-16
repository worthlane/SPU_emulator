#include <stdio.h>

#include "disassembler.h"
#include "../common/asm_funcs.h"
#include "../common/log_funcs.h"
#include "../common/errors.h"

static const char* DEFAULT_OUT = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/asm_code.txt";
static const char* DEFAULT_IN  = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.bin";

int main(const int argc, const char* argv[])
{
    OpenLogFile(argv[0]);

    ErrorInfo error = {};

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

    Storage info = {};

    FILE* in_stream  = OpenByteCodeFile(input_file, &error);
    EXIT_IF_ERROR(&error);

    FILE* out_stream = OpenOutputFile(output_file, &error);
    EXIT_IF_ERROR(&error);

    AsmErrors asm_err = DisAssembly(in_stream, out_stream);
    if (asm_err != AsmErrors::NONE)
    {
        error.code = ERRORS::DISASM_ERROR;
        EXIT_IF_ERROR(&error);
    }

    fclose(in_stream);
    fclose(out_stream);
}
