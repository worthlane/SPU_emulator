#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <stdio.h>

#include "../common/commands.h"
#include "../common/input_and_output.h"

CommandErrors HandleCommand(FILE* in_stream, FILE* out_stream, Storage* info);

#endif
