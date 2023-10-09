#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "commands.h"
#include "types.h"

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

char* AddSignature(char* current_byte)
{
    assert(current_byte);

    current_byte += sprintf(current_byte, "%llu %d\n", SIGNATURE, ASM_VER);

    return current_byte;
}

//------------------------------------------------------------------

CommandErrors VerifySignature(char* buf, const signature_t expected_sign, const int expected_ver)
{
    assert(buf);

    CommandErrors error = CommandErrors::OK;

    signature_t sign = 0;
    int         ver  = 0;

    sscanf(buf, "%llu %d", &sign, &ver);

    if (sign != expected_sign)
        return CommandErrors::INCORRECT_SIGNATURE;  // TODO обернуть

    if (ver != expected_ver)
        return CommandErrors::INCORRECT_VERSION;

    return error;
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

CommandErrors RegVerify(RegisterCode reg)
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
            return CommandErrors::INVALID_REGISTER;
    }

    return CommandErrors::OK;
}

// --------------------------------------------------------

bool SyntaxCheckRemainingString(const char* const source)
{
    assert(source);

    const char* src = source;

    while (*src != '\n' && *src != EOF && *src != '\0')
    {
        putchar(*src);
        if (!isspace(*src))
            return false;
        src++;
    }

    putchar(*src);

    return true;
}
