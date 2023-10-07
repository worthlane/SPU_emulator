#include <strings.h>
#include <assert.h>

#include "disassembler.h"
#include "../common/log_funcs.h"


static void PrintRemainingString(FILE* in_stream, FILE* out_stream);
static void ClearInput(FILE* fp);

CommandErrors HandleCode(FILE* in_stream, FILE* out_stream)
{
    assert(in_stream);
    assert(out_stream);

    CommandErrors error = CommandErrors::OK;
    int command = 0;
    CommandCode command_code = CommandCode::hlt;

    while (true)
    {
        fscanf(in_stream,"%d", &command);

        command_code = (CommandCode) command;

        switch (command_code)
        {
            case (CommandCode::push):
                fprintf(out_stream, "%s", PUSH);
                break;
            case (CommandCode::in):
                fprintf(out_stream, "%s", IN);
                break;
            case (CommandCode::out):
                fprintf(out_stream, "%s", OUT);
                break;
            case (CommandCode::sub):
                fprintf(out_stream, "%s", SUB);
                break;
            case (CommandCode::add):
                fprintf(out_stream, "%s", ADD);
                break;
            case (CommandCode::mul):
                fprintf(out_stream, "%s", MUL);
                break;
            case (CommandCode::div):
                fprintf(out_stream, "%s", DIV);
                break;
            case (CommandCode::sqrt):
                fprintf(out_stream, "%s", SQRT);
                break;
            case (CommandCode::sin):
                fprintf(out_stream, "%s", SIN);
                break;
            case (CommandCode::cos):
                fprintf(out_stream, "%s", OUT);
                break;
            case (CommandCode::hlt):
                fprintf(out_stream, "%s", HLT);
                return CommandErrors::OK;
            case (CommandCode::unk):
                // fall through
            default:
                PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                         "ERROR: UNKNOWN CODE \"%d\"\n"
                         "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", command_code); // TODO обернуть нормально потом

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
