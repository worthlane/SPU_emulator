#ifndef __DISASSEMBLER_H_
#define __DISASSEMBLER_H_

#include <stdio.h>

#include "../common/asm_common.h"
#include "../common/input_and_output.h"

AsmErrors DisAssembly(FILE* in_stream, FILE* out_stream);

#endif
