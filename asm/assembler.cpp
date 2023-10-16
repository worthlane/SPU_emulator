#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "assembler.h"
#include "../common/log_funcs.h"

static RegisterCode GetRegister(char* input_ptr, size_t* extra_cmd_sym);
static AsmErrors HandlePushCommand(PushInfo* push, size_t* extra_cmd_sym, char* input_byte);
static size_t CountElemLen(elem_t value);

static inline void PrintBytesInTXT(FILE* out_stream, int64_t* byte_buf, size_t byte_amt);
static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream);

AsmErrors Assembly(FILE* in_stream, FILE* out_stream, FILE* out_bin_stream, Storage* info)
{
    assert(in_stream);
    assert(out_stream);

    AsmErrors error = AsmErrors::NONE;

    int64_t* byte_buf = (int64_t*) calloc(MAX_BYTE_CODE_LEN, sizeof(int64_t));
    size_t   position = 0;

    if (byte_buf == nullptr)
        error = AsmErrors::ALLOCATE_MEM;

    RETURN_IF_ASMERROR(error);

    AddSignature(byte_buf, &position);

    char command[MAX_COMMAND_LEN] = "";

    for (size_t line = 0; line < info->line_amt; line++)
    {
        sscanf(info->lines[line].string, "%s", command);
        size_t cmd_len       = strlen(command);
        size_t extra_cmd_sym = 0;

        if (!strncmp(command, HLT, MAX_COMMAND_LEN))
        {
            byte_buf[position++] = (int64_t) CommandCode::HLT_ID;
            break;
        }
        else if (!strncmp(command, POP, MAX_COMMAND_LEN))
        {
            byte_buf[position++] = (int64_t) CommandCode::POP_ID;

            RegisterCode reg_code = GetRegister(info->lines[line].string + cmd_len,
                                                &extra_cmd_sym);
            if (reg_code == RegisterCode::unk)
                error    =  AsmErrors::INVALID_REGISTER;
            RETURN_IF_ASMERROR(error);

            byte_buf[position++] = (int64_t) reg_code;
        }
        else if (!strncmp(command, OUT, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::OUT_ID;
        else if (!strncmp(command, PUSH, MAX_COMMAND_LEN))
        {
            byte_buf[position++] = (int64_t) CommandCode::PUSH_ID;

            PushInfo push = {};

            error = HandlePushCommand(&push, &extra_cmd_sym,
                                      info->lines[line].string + cmd_len);
            RETURN_IF_ASMERROR(error);

            byte_buf[position++] = push.reg;
            byte_buf[position++] = push.val;
        }
        else if (!strncmp(command, IN, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::IN_ID;
        else if (!strncmp(command, SPEAK, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::SPEAK_ID;

        else if (!strncmp(command, SUB, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::SUB_ID;
        else if (!strncmp(command, ADD, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::ADD_ID;
        else if (!strncmp(command, MUL, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::MUL_ID;
        else if (!strncmp(command, DIV, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::DIV_ID;
        else if (!strncmp(command, SQRT, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::SQRT_ID;
        else if (!strncmp(command, SIN, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::SIN_ID;
        else if (!strncmp(command, COS, MAX_COMMAND_LEN))
            byte_buf[position++] = (int64_t) CommandCode::COS_ID;
        else
        {
            error = AsmErrors::UNKNOWN_WORD;
        }
        RETURN_IF_ASMERROR(error);

        error = SyntaxCheckRemainingString(info->lines[line].string + cmd_len + extra_cmd_sym);
        RETURN_IF_ASMERROR(error);
    }

    PrintBytesInBIN(byte_buf, sizeof(*byte_buf), position, out_bin_stream);
    PrintBytesInTXT(out_stream, byte_buf, position);

    free(byte_buf);

    return AsmErrors::NONE;
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

static AsmErrors HandlePushCommand(PushInfo* push, size_t* extra_cmd_sym, char* input_byte)
{
    elem_t value = 0;
    int read     = sscanf(input_byte, "%ld", &value);

    if (read == 0)
    {
        RegisterCode reg_code = GetRegister(input_byte, extra_cmd_sym);
        if (reg_code == RegisterCode::unk)
            return AsmErrors::INVALID_REGISTER;

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

    return AsmErrors::NONE;
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

//------------------------------------------------------------------

static inline void PrintBytesInTXT(FILE* out_stream, int64_t* byte_buf, size_t byte_amt)
{
    assert(out_stream);
    assert(byte_buf);

    for (size_t i = 0; i < byte_amt; i++)
        fprintf(out_stream, "%016llX\n", byte_buf[i]);
}

//------------------------------------------------------------------

static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream)
{
    assert(out_stream);
    assert(buf);

    fwrite(buf, size, amt, out_stream);
}

