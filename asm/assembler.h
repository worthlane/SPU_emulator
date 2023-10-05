#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <stdio.h>

#include "commands.h"

CommandErrors HandleCommand(FILE* in_stream, FILE* out_stream);

#endif
