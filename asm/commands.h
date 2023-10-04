#ifndef __COMMANDS_H_
#define __COMMANDS_H_

enum class CommandCode
{
    hlt  = -1,

    out  =  0,

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

    UNKNOWN_CODE,
    UNKNOWN_WORD
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

#endif
