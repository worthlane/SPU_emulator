#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "asm_common.h"
#include "types.h"
#include "errors.h"

char* PrintRemainingString(const char* const source, char* destination)
{
    assert(source);
    assert(destination);

    const char* src = source;
    char* dest = destination;

    while (*src != '\n' && *src != EOF && *src != '\0')
    {
        *dest = *src;

        src++;
        dest++;
    }

    *dest = *src;
    dest++;

    return dest;
}

//------------------------------------------------------------------

void AddSignature(code_t* byte_buf, size_t* position)
{
    assert(byte_buf);

    byte_buf[(*position)++] = (code_t) SIGNATURE;
    byte_buf[(*position)++] = (code_t) ASM_VER;
}

//------------------------------------------------------------------

AsmErrors VerifySignature(code_t* byte_buf, size_t* position,
                          const code_t expected_sign, const int expected_ver)
{
    assert(byte_buf);
    assert(position);

    code_t sign = byte_buf[(*position)++];
    code_t ver  = byte_buf[(*position)++];

    if (sign != expected_sign)
        return AsmErrors::INCORRECT_SIGNATURE;

    if (ver != expected_ver)
        return AsmErrors::INCORRECT_VERSION;

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

RegisterCode TranslateRegisterToByte(const char* reg)
{
    assert(reg);

    if (!strncmp(reg, RAX, MAX_REG_LEN))
        return RegisterCode::rax;
    else if (!strncmp(reg, RBX, MAX_REG_LEN))
        return RegisterCode::rbx;
    else if (!strncmp(reg, RCX, MAX_REG_LEN))
        return RegisterCode::rcx;
    else if (!strncmp(reg, RDX, MAX_REG_LEN))
        return RegisterCode::rdx;
    else
        return RegisterCode::unk;
}

//------------------------------------------------------------------

AsmErrors VerifyRegister(RegisterCode reg)
{
    switch (reg)
    {
        case (RegisterCode::rax):
            break;
        case (RegisterCode::rbx):
            break;
        case (RegisterCode::rcx):
            break;
        case (RegisterCode::rdx):
            break;
        case (RegisterCode::unk):
            // fall through
        default:
            return AsmErrors::INVALID_REGISTER;
    }

    return AsmErrors::NONE;
}

// --------------------------------------------------------

AsmErrors SyntaxCheckRemainingString(const char* const source)
{
    assert(source);

    const char* src = source;

    while (*src != '\n' && *src != EOF && *src != '\0')
    {
        if (!isspace(*src))
            return AsmErrors::SYNTAX_ERROR;
        src++;
    }

    return AsmErrors::NONE;
}

// --------------------------------------------------------

int DumpAsmError(FILE* fp, const void* err, const char* func, const char* file, const int line)
{
    AsmErrors error = *((AsmErrors*) err);

    LOG_START_MOD(func, file, line);

    switch (error)
    {
        case (AsmErrors::NONE):
            break;

        case (AsmErrors::ALLOCATE_MEM):
            fprintf(fp, "\nERROR: FAILED TO ALLOCATE MEMORY\n\n");
            break;

        case (AsmErrors::SYNTAX_ERROR):
            fprintf(fp, "\nERROR: SYNTAX ERROR IN\n\n");
            break;

        case (AsmErrors::UNKNOWN_CODE):
            fprintf(fp, "\nERROR: UNKNOWN COMMAND CODE\n\n");
            break;

        case (AsmErrors::UNKNOWN_WORD):
            fprintf(fp, "\nERROR: UNKNOWN COMMAND WORD\n\n");
            break;

        case (AsmErrors::INCORRECT_SIGNATURE):
            fprintf(fp, "\nERROR: INCORRECT SIGNATURE\n\n");
            break;

        case (AsmErrors::INCORRECT_VERSION):
            fprintf(fp, "\nERROR: INCORRECT VERSION\n\n");
            break;

        case (AsmErrors::INVALID_REGISTER):
            fprintf(fp, "\nERROR: INVALID REGISTER\n\n");
            break;

        case (AsmErrors::READ_BYTE_CODE):
            fprintf(fp, "\nERROR: CAN NOT READ BYTE CODE FILE\n\n");
            break;

        case (AsmErrors::PRINT_VALUE):
            fprintf(fp, "\nERROR: CAN NOT PRINT VALUE\n\n");
            break;

        case (AsmErrors::TOO_MANY_LABELS):
            fprintf(fp, "\nERROR: TOO MANY LABELS\n\n");
            break;

        case (AsmErrors::UNKNOWN_LABEL):
            fprintf(fp, "\nERROR: UNKNOWN LABEL\n\n");
            break;

        default:
            fprintf(fp, "\nERROR: UNKNOWN ASM ERROR\n\n");
            break;
    }

    LOG_END();

    return (int) error;
}

//-------------------------------------------------------------------------------------------

FILE* OpenByteCodeFile(const char* input_file, ErrorInfo* error)
{
    FILE* in_stream  = fopen(input_file, "rb");

    if (in_stream == nullptr)
    {
        error->code = ERRORS::OPEN_FILE;
        error->data = (void*) input_file;
        return in_stream;
    }

    return in_stream;
}

//-------------------------------------------------------------------------------------------

AsmErrors TranslateByteToRegister(const RegisterCode reg, char* register_name)
{
    assert(register_name);

    switch (reg)
    {
        case (RegisterCode::rax):
            strncpy(register_name, RAX, MAX_REG_LEN);
            break;

        case (RegisterCode::rbx):
            strncpy(register_name, RBX, MAX_REG_LEN);
            break;

        case (RegisterCode::rcx):
            strncpy(register_name, RCX, MAX_REG_LEN);
            break;

        case (RegisterCode::rdx):
            strncpy(register_name, RDX, MAX_REG_LEN);
            break;

        default:
            return AsmErrors::INVALID_REGISTER;
    }

    return AsmErrors::NONE;
}
