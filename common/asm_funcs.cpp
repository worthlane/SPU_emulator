#include <assert.h>

#include "commands.h"

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
