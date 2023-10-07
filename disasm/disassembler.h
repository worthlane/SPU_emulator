#ifndef __DISASSEMBLER_H_
#define __DISASSEMBLER_H_

#include <stdio.h>

#include "../common/commands.h"
#include "../common/input_and_output.h"

CommandErrors HandleCode(FILE* in_stream, FILE* out_stream, Storage* info);

#endif
