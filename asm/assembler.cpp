#include <strings.h>
#include <assert.h>

#include "assembler.h"
#include "../log_funcs.h"

static void PrintRemainingString(FILE* in_stream, FILE* out_stream);
static void ClearInput(FILE* fp);

CommandErrors HandleCommand(FILE* in_stream, FILE* out_stream)
{
    assert(in_stream);
    assert(out_stream);

    char command[MAX_COMMAND_LEN] = "";

    while (true)
    {
        fscanf(in_stream,"%s", command);

        if (!strncmp(command, HLT, MAX_COMMAND_LEN))
        {
            fprintf(out_stream, "%d", (int) CommandCode::hlt);
            break;
        }
        else if (!strncmp(command, OUT, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::out);
        else if (!strncmp(command, PUSH, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::push);
        else if (!strncmp(command, IN, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::in);
        else if (!strncmp(command, SUB, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::sub);
        else if (!strncmp(command, ADD, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::add);
        else if (!strncmp(command, MUL, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::mul);
        else if (!strncmp(command, DIV, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::div);
        else if (!strncmp(command, SQRT, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::sqrt);
        else if (!strncmp(command, SIN, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::sin);
        else if (!strncmp(command, COS, MAX_COMMAND_LEN))
            fprintf(out_stream, "%d", (int) CommandCode::cos);
        else
        {
            PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                     "ERROR: UNKNOWN WORD \"%s\"\n"
                     "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", command); // TODO обернуть нормально потом

            return CommandErrors::UNKNOWN_WORD;
        }

        PrintRemainingString(in_stream, out_stream);
        fputc('\n', out_stream);
    }

    return CommandErrors::OK;
}

//------------------------------------------------------------------

static void PrintRemainingString(FILE* in_stream, FILE* out_stream)
{
    assert(in_stream);
    assert(out_stream);

    int ch = 0;
    while ((ch = fgetc(in_stream)) != '\n' && ch != EOF)
    {
        fputc(ch, out_stream);
    }
}

//------------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}
