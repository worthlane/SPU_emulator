#include <strings.h>
#include <assert.h>

#include "assembler.h"
#include "../common/log_funcs.h"

static void ClearInput(FILE* fp);

CommandErrors HandleCommand(FILE* in_stream, FILE* out_stream, Storage* info)
{
    assert(in_stream);
    assert(out_stream);

    char* byte_buf = (char*) calloc(info->text_len, sizeof(char));
    char* current_byte = byte_buf;

    if (byte_buf == nullptr)
    {
        PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                 "ERROR: FAILED TO ALLOCATE MEMORY\n"
                 "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"); // TODO обернуть нормально потом

        return CommandErrors::ALLOCATE_MEM;
    }

    char command[MAX_COMMAND_LEN] = "";

    for (size_t line = 0; line < info->line_amt; line++)
    {
        sscanf(info->lines[line].string,"%s", command);

        if (!strncmp(command, HLT, MAX_COMMAND_LEN))
        {
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::hlt);
            break;
        }
        else if (!strncmp(command, OUT, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::out);
        else if (!strncmp(command, PUSH, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::push);
        else if (!strncmp(command, IN, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::in);
        else if (!strncmp(command, SUB, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::sub);
        else if (!strncmp(command, ADD, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::add);
        else if (!strncmp(command, MUL, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::mul);
        else if (!strncmp(command, DIV, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::div);
        else if (!strncmp(command, SQRT, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::sqrt);
        else if (!strncmp(command, SIN, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::sin);
        else if (!strncmp(command, COS, MAX_COMMAND_LEN))
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::cos);
        else
        {
            PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                     "ERROR: UNKNOWN WORD \"%s\"\n"
                     "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", command); // TODO обернуть нормально потом

            return CommandErrors::UNKNOWN_WORD;
        }

        current_byte = PrintRemainingString(info->lines[line].string + strlen(command), current_byte);
    }

    PrintBuf(out_stream, byte_buf, info->text_len);

    free(byte_buf);

    return CommandErrors::OK;
}

//------------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}
