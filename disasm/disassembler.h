#ifndef __DISASSEMBLER_H_
#define __DISASSEMBLER_H_

#include <stdio.h>

#include "../common/commands.h"

CommandErrors HandleCode(FILE* in_stream, FILE* out_stream);

#endif
