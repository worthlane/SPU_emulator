#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include <stdio.h>

enum class CommandCode
{
    hlt  = -1,

    out  =  0,

    push =  1,
    in   =  2,

    sub  =  3,
    add  =  4,
    mul  =  5,
    div  =  6,
    sqrt =  7,
    sin  =  8,
    cos  =  9,

    unk  = 10
};

static const size_t MAX_COMMAND_LEN = 10;

//--------------COMMANDS LIST------------

static const char* HLT  = "hlt";

static const char* OUT  = "out";

static const char* PUSH = "push";
static const char* IN   = "in";

static const char* SUB  = "sub";
static const char* ADD  = "add";
static const char* MUL  = "mul";
static const char* DIV  = "div";
static const char* SQRT = "sqrt";
static const char* SIN  = "sin";
static const char* COS  = "cos";

//---------------------------------------

CommandCode HandleCommand(const char* command);
void ClearInput(FILE* fp);

#endif
