#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "assembler.h"
#include "../common/log_funcs.h"

static RegisterCode GetRegister(char* input_ptr, size_t* extra_cmd_sym);
static CommandErrors HandlePushCommand(PushInfo* push, size_t* extra_cmd_sym, char* input_byte);
static size_t CountElemLen(elem_t value);

CommandErrors HandleCommand(FILE* in_stream, FILE* out_stream, Storage* info)
{
    assert(in_stream);
    assert(out_stream);

    char* byte_buf = (char*) calloc(info->text_len + SIGNATURE_LEN, sizeof(char));
    char* current_byte = byte_buf;

    current_byte = AddSignature(current_byte);

    if (byte_buf == nullptr)
    {
        PrintLog("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
                 "ERROR: FAILED TO ALLOCATE MEMORY\n"
                 "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"); // TODO обернуть нормально потом

        return CommandErrors::ALLOCATE_MEM;
    }

    char   command[MAX_COMMAND_LEN] = "";
    size_t extra_cmd_sym            = 0;

    for (size_t line = 0; line < info->line_amt; line++)
    {
        sscanf(info->lines[line].string,"%s", command);
        size_t cmd_len = strlen(command);

        if (!strncmp(command, HLT, MAX_COMMAND_LEN))                                // vvvvvvvvvv HLT CMD
        {
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::hlt);
            break;
        }                                                                           // ^^^^^^^^^^^^^^^^^^^
        else if (!strncmp(command, POP, MAX_COMMAND_LEN))                           // vvvvvvvvvv POP COMMAND
        {
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::pop);

            RegisterCode reg_code = GetRegister(info->lines[line].string + cmd_len,
                                                &extra_cmd_sym);
            if (reg_code == RegisterCode::unk)
                return CommandErrors::INVALID_REGISTER;

            current_byte += sprintf(current_byte, " %d", (int) reg_code);
        }                                                                           // ^^^^^^^^^^^^^^^^^^^^^
        else if (!strncmp(command, OUT, MAX_COMMAND_LEN))                           // vvvvvvvvvvvvv OUT CMD
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::out);    // ^^^^^^^^^^^^^^^^^^^^^
        else if (!strncmp(command, PUSH, MAX_COMMAND_LEN))                          // vvvvvvvvvvvvv PUSH COMMAND
        {
            current_byte += sprintf(current_byte, "%d", (int) CommandCode::push);

            PushInfo push = {};

            CommandErrors err = HandlePushCommand(&push, &extra_cmd_sym,
                                                  info->lines[line].string + cmd_len);

            current_byte += sprintf(current_byte, " %d %ld", push.reg, push.val);
        }                                                                           // ^^^^^^^^^^^^^^^^^^^^^^^^^^
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

        current_byte = PrintRemainingString(info->lines[line].string + cmd_len + extra_cmd_sym,
                                            current_byte);
    }

    PrintBuf(out_stream, byte_buf, info->text_len + SIGNATURE_LEN);         // TODO может быть сегфолт

    free(byte_buf);

    return CommandErrors::OK;
}

//------------------------------------------------------------------

static RegisterCode GetRegister(char* input_ptr, size_t* extra_cmd_sym)
{
    assert(input_ptr);
    assert(extra_cmd_sym);

    char reg[MAX_REG_LEN] = "";
    sscanf(input_ptr, "%s", reg);

    // space between cmd and reg---v
    *extra_cmd_sym = strlen(reg) + 1;

    RegisterCode reg_code = TranslateRegisterToByte(reg);

    return reg_code;
}

//------------------------------------------------------------------

static CommandErrors HandlePushCommand(PushInfo* push, size_t* extra_cmd_sym, char* input_byte)
{
    elem_t value = 0;
    int read     = sscanf(input_byte, "%ld", &value);

    if (read == 0)
    {
        RegisterCode reg_code = GetRegister(input_byte, extra_cmd_sym);
        if (reg_code == RegisterCode::unk)
            return CommandErrors::INVALID_REGISTER;

        push->reg = true;
        push->val = (int) reg_code;
    }
    else
    {
        push->reg = false;
        push->val = value;

        //               v---- space betwen cmd and value
        *extra_cmd_sym = 1 + CountElemLen(push->val);
    }

    return CommandErrors::OK;
}

//------------------------------------------------------------------

static size_t CountElemLen(elem_t value)
{
    size_t cnt = 0;

    if (value < 0)
        cnt++;

    do
    {
        cnt++;
        value /= 10;
    }
    while (value);

    return cnt;
}



