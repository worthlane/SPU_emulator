#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <stdio.h>

#include "../common/asm_funcs.h"
#include "../common/input_and_output.h"

AsmErrors Assembly(FILE* in_stream, FILE* out_stream, FILE* out_bin_stream, Storage* info);

#endif
