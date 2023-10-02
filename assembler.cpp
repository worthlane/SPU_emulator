#include <strings.h>

#include "assembler.h"

CommandCode HandleCommand(const char* command)
{
    if (!strncmp(command, HLT, MAX_COMMAND_LEN))
        return CommandCode::hlt;
    else if (!strncmp(command, OUT, MAX_COMMAND_LEN))
        return CommandCode::out;
    else if (!strncmp(command, PUSH, MAX_COMMAND_LEN))
        return CommandCode::push;
    else if (!strncmp(command, IN, MAX_COMMAND_LEN))
        return CommandCode::in;
    else if (!strncmp(command, SUB, MAX_COMMAND_LEN))
        return CommandCode::sub;
    else if (!strncmp(command, ADD, MAX_COMMAND_LEN))
        return CommandCode::add;
    else if (!strncmp(command, MUL, MAX_COMMAND_LEN))
        return CommandCode::mul;
    else if (!strncmp(command, DIV, MAX_COMMAND_LEN))
        return CommandCode::div;
    else if (!strncmp(command, SQRT, MAX_COMMAND_LEN))
        return CommandCode::sqrt;
    else if (!strncmp(command, SIN, MAX_COMMAND_LEN))
        return CommandCode::sin;
    else if (!strncmp(command, COS, MAX_COMMAND_LEN))
        return CommandCode::cos;
    else
        return CommandCode::unk;
}

//------------------------------------------------------------------

void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}
