#include <strings.h>
#include <assert.h>
#include <stdlib.h>

#include "disassembler.h"
#include "../common/log_funcs.h"

static void ClearInput(FILE* fp);

CommandErrors HandleCode(FILE* in_stream, FILE* out_stream, Storage* info)
{
    assert(in_stream);
    assert(out_stream);

    CommandErrors error = VerifySignature(info->buf, SIGNATURE, ASM_VER);
    if (error != CommandErrors::OK)
        return error;

    char command[10] = "";
    CommandCode command_code = CommandCode::hlt;

    char* asm_buf = (char*) calloc(info->text_len, sizeof(char));
    char* current_byte = asm_buf;

    if (asm_buf == nullptr)
    {
        PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                 "ERROR: FAILED TO ALLOCATE MEMORY\n"
                 "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"); // TODO обернуть нормально потом

        return CommandErrors::ALLOCATE_MEM;
    }

    for (size_t line = 1; line < info->line_amt; line++)
    {
        sscanf(info->lines[line].string, "%s", command);
        size_t code_len = strlen(command);

        long command_id = strtol(command, nullptr, 10);

        command_code = (CommandCode) command_id;

        bool  quit_cycle_flag = false;

        switch (command_code)
        {
            case (CommandCode::push):
                current_byte += sprintf(current_byte, "%s", PUSH);
                break;
            case (CommandCode::in):
                current_byte += sprintf(current_byte, "%s", IN);
                break;
            case (CommandCode::out):
                current_byte += sprintf(current_byte, "%s", OUT);
                break;
            case (CommandCode::sub):
                current_byte += sprintf(current_byte, "%s", SUB);
                break;
            case (CommandCode::add):
                current_byte += sprintf(current_byte, "%s", ADD);
                break;
            case (CommandCode::mul):
                current_byte += sprintf(current_byte, "%s", MUL);
                break;
            case (CommandCode::div):
                current_byte += sprintf(current_byte, "%s", DIV);
                break;
            case (CommandCode::sqrt):
                current_byte += sprintf(current_byte, "%s", SQRT);
                break;
            case (CommandCode::sin):
                current_byte += sprintf(current_byte, "%s", SIN);
                break;
            case (CommandCode::cos):
                current_byte += sprintf(current_byte, "%s", OUT);
                break;
            case (CommandCode::hlt):
                current_byte += sprintf(current_byte, "%s", HLT);
                quit_cycle_flag = true;
                break;
            case (CommandCode::unk):
                // fall through
            default:
                PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                         "ERROR: UNKNOWN CODE \"%d\"\n"
                         "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n", command_code); // TODO обернуть нормально потом

            return CommandErrors::UNKNOWN_WORD;
        }

        if (quit_cycle_flag)
            break;

        current_byte = PrintRemainingString(info->lines[line].string + code_len,
                                            current_byte);
    }

    PrintBuf(out_stream, asm_buf, info->text_len);

    free(asm_buf);

    return CommandErrors::OK;
}

//------------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}
