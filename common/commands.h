#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include <stdio.h>

enum class CommandCode
{
    hlt  = -1,

    out  =  228,

    push,
    in,

    sub,
    add,
    mul,
    div,
    sqrt,
    sin,
    cos,

    unk
};

enum class CommandErrors
{
    OK = 0,

    ALLOCATE_MEM,

    UNKNOWN_CODE,
    UNKNOWN_WORD
};

static const size_t MAX_COMMAND_LEN = 10;

static const char* CODE_WORD = "deco";

//=============REGISTERS================

static const char* RAX = "rax";
static const char* RBX = "rbx";
static const char* RCX = "rcx";
static const char* RDX = "rdx";

enum RegisterCode
{
    rax = 0,
    rbx,
    rcx,
    rdx
};

static const size_t REG_AMT = 4;

//======================================

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

char* PrintRemainingString(const char* const source, char* dest);

#endif
