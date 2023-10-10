#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include <stdio.h>

#include "types.h"
#include "input_and_output.h"
#include "log_funcs.h"

enum class CommandCode
{
    hlt  = -1,

    out  =  1000 - 7,

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

#ifdef RETURN_IF_ASMERROR
#undef RETURN_IF_ASMERROR

#endif
#define RETURN_IF_ASMERROR(error)           do                                                          \
                                            {                                                           \
                                                if ((error) != AsmErrors::NONE)                         \
                                                {                                                       \
                                                    LogDump(DumpAsmError, &error,                       \
                                                            __func__, __FILE__, __LINE__);              \
                                                    return error;                                       \
                                                }                                                       \
                                            } while(0)

static const size_t MAX_BYTE_CODE_LEN = 1000;
static const size_t MAX_ASM_CODE_LEN  = 10000;

enum class AsmErrors
{
    NONE = 0,

    ALLOCATE_MEM,

    READ_BYTE_CODE,
    PRINT_VALUE,

    SYNTAX_ERROR,
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

static const size_t REG_AMT      = 4;
static const size_t MAX_REG_LEN  = 5;
static const size_t MAX_WORD_LEN = 64;

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
AsmErrors SyntaxCheckRemainingString(const char* const source);

void AddSignature(int64_t* byte_buf, size_t* position);
AsmErrors VerifySignature(int64_t* byte_buf, size_t* position,
                          const signature_t expected_sign, const int expected_ver);

RegisterCode TranslateRegisterToByte(const char* reg);
AsmErrors TranslateByteToRegister(const RegisterCode reg, char* register_name);
AsmErrors VerifyRegister(RegisterCode reg);

int DumpAsmError(FILE* fp, const void* err, const char* func, const char* file, const int line);

FILE* OpenByteCodeFile(const char* input_file, ErrorInfo* error);

#endif
