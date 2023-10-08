#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include <stdio.h>

#include "types.h"
#include "input_and_output.h"

enum class CommandCode
{
    hlt  = -1,

    out  =  228,

    push,
    in,

    pop,

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
    UNKNOWN_WORD,

    INCORRECT_SIGNATURE,
    INCORRECT_VERSION,

    INVALID_REGISTER
};

static const signature_t SIGNATURE = 'DEC0';
static const int         ASM_VER   = 2;

//           code word len ---------v        v----- version len
static const size_t SIGNATURE_LEN = 10 + 2 + 1;
//     space + next string symbols ------^

//=============REGISTERS================

static const char* RAX = "rax";
static const char* RBX = "rbx";
static const char* RCX = "rcx";
static const char* RDX = "rdx";

enum RegisterCode
{
    unk = -1,

    rax =  0,
    rbx,
    rcx,
    rdx
};

static const size_t REG_AMT     = 4;
static const size_t MAX_REG_LEN = 5;

//======================================

static const size_t MAX_COMMAND_LEN = 10;

struct PushInfo
{
    unsigned int reg : 1;
    elem_t       val;
};

//--------------COMMANDS LIST------------

static const char* HLT  = "hlt";

static const char* OUT  = "out";

static const char* PUSH = "push";
static const char* IN   = "in";

static const char* POP  = "pop";

static const char* SUB  = "sub";
static const char* ADD  = "add";
static const char* MUL  = "mul";
static const char* DIV  = "div";
static const char* SQRT = "sqrt";
static const char* SIN  = "sin";
static const char* COS  = "cos";

//---------------------------------------

char* PrintRemainingString(const char* const source, char* dest);

char* AddSignature(char* current_byte);
CommandErrors VerifySignature(char* buf, const signature_t expected_sign, const int expected_ver);
RegisterCode TranslateRegisterToByte(const char* reg);
CommandErrors RegVerify(RegisterCode reg);

#endif
