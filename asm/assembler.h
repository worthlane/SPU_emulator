#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <stdio.h>

#include "../common/asm_common.h"
#include "../common/input_and_output.h"

AsmErrors Assembly(FILE* out_stream, FILE* out_bin_stream, Storage* info);

struct LabelInfo
{
    char*  label_name;
    size_t jmp_byte;
};

typedef struct LabelInfo label_t;

static const size_t MAX_LABELS_AMT = 10;
static const size_t MAX_LABEL_NAME = 100;

#endif
