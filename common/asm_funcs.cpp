#include <assert.h>

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

