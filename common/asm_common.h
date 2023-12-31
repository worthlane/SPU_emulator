#ifndef __COMMANDS_H_
#define __COMMANDS_H_

#include <stdio.h>

#include "types.h"
#include "input_and_output.h"
#include "logs.h"

#define DEF_CMD(name, num, ...) \
          ID_##name = num,

enum class CommandCode
{
    #include "commands.h"
};

#undef DEF_CMD

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

    TOO_MANY_LABELS,
    UNKNOWN_LABEL,

    INVALID_REGISTER
};

static const code_t SIGNATURE = 'DEC0';
static const code_t ASM_VER   = 2;
static const size_t BYTE_AFTER_SIGNATURE = 2 * sizeof(int);

//           code word len ---------v        v----- version len
static const size_t SIGNATURE_LEN = 10 + 2 + 1;
//     space + next string symbols ------^

//=============REGISTERS================


#pragma GCC diagnostic ignored "-Wunused-variable"
static const char* RAX = "rax";
static const char* RBX = "rbx";
static const char* RCX = "rcx";
static const char* RDX = "rdx";
#pragma GCC diagnostic warning "-Wunused-variable"

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

enum ArgumentType
{
    NONE = 0,

    HAS_LABELS,

    NO_LABELS,
};

struct PushInfo
{
    unsigned int reg : 1;
    elem_t       val;
};

//---------------------------------------

static const int RAM_ARG   = 1 << 5;
static const int NUM_ARG   = 1 << 6;
static const int REG_ARG   = 1 << 7;

char* PrintRemainingString(const char* const source, char* dest);
AsmErrors SyntaxCheckRemainingString(const char* const source);

void AddSignature(code_t* byte_buf, size_t* position);
AsmErrors VerifySignature(code_t* byte_buf, size_t* position,
                          const code_t expected_sign, const int expected_ver);

RegisterCode TranslateRegisterToByte(const char* reg);
AsmErrors TranslateByteToRegister(const RegisterCode reg, char* register_name);
AsmErrors VerifyRegister(RegisterCode reg);

int DumpAsmError(FILE* fp, const void* err, const char* func, const char* file, const int line);

FILE* OpenByteCodeFile(const char* input_file, ErrorInfo* error);

#endif
