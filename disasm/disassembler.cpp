#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "disassembler.h"
#include "../common/log_funcs.h"

static void ClearInput(FILE* fp);
static AsmErrors ReadPushTarget(const int64_t* byte_buf, size_t* position, char* val);

AsmErrors DisAssembly(FILE* in_stream, FILE* out_stream)
{
    assert(in_stream);
    assert(out_stream);

    AsmErrors error = AsmErrors::NONE;

    // =============================== BYTE BUFFER INIT
    int64_t byte_buf[MAX_BYTE_CODE_LEN] = {};
    size_t  size     = fread(byte_buf, sizeof(int64_t), MAX_BYTE_CODE_LEN, in_stream);
    size_t  position = 0;
    if (size == 0)
        error = AsmErrors::READ_BYTE_CODE;
    RETURN_IF_ASMERROR(error);
    // =====================================================

    // ================================= ASM BUFFER INIT
    char* asm_buf      = (char*) calloc(MAX_ASM_CODE_LEN, sizeof(char));
    char* current_byte = asm_buf;
    if (asm_buf == nullptr)
        error = AsmErrors::ALLOCATE_MEM;
    RETURN_IF_ASMERROR(error);
    // =================================================

    error = VerifySignature(byte_buf, &position, SIGNATURE, ASM_VER);
    RETURN_IF_ASMERROR(error);

    CommandCode cmd_code = CommandCode::HLT_ID;

    while(true)
    {
        cmd_code = (CommandCode) byte_buf[position++];
        bool  quit_cycle_flag = false;

        switch (cmd_code)
        {
            case (CommandCode::PUSH_ID):
            {
                current_byte += sprintf(current_byte, "%s", PUSH);

                char val[MAX_WORD_LEN] = "";
                error = ReadPushTarget(byte_buf, &position, val);
                RETURN_IF_ASMERROR(error);

                current_byte += sprintf(current_byte, " %s", val);
                break;
            }
            case (CommandCode::SPEAK_ID):
                current_byte += sprintf(current_byte, "%s", SPEAK);
                break;
            case (CommandCode::IN_ID):
                current_byte += sprintf(current_byte, "%s", IN);
                break;
            case (CommandCode::OUT_ID):
                current_byte += sprintf(current_byte, "%s", OUT);
                break;
            case (CommandCode::SUB_ID):
                current_byte += sprintf(current_byte, "%s", SUB);
                break;
            case (CommandCode::ADD_ID):
                current_byte += sprintf(current_byte, "%s", ADD);
                break;
            case (CommandCode::MUL_ID):
                current_byte += sprintf(current_byte, "%s", MUL);
                break;
            case (CommandCode::DIV_ID):
                current_byte += sprintf(current_byte, "%s", DIV);
                break;
            case (CommandCode::SQRT_ID):
                current_byte += sprintf(current_byte, "%s", SQRT);
                break;
            case (CommandCode::SIN_ID):
                current_byte += sprintf(current_byte, "%s", SIN);
                break;
            case (CommandCode::COS_ID):
                current_byte += sprintf(current_byte, "%s", OUT);
                break;
            case (CommandCode::POP_ID):
            {
                current_byte += sprintf(current_byte, "%s", POP);

                RegisterCode reg = (RegisterCode) byte_buf[position++];
                char register_name[MAX_REG_LEN] = "";

                error = TranslateByteToRegister(reg, register_name);
                RETURN_IF_ASMERROR(error);

                current_byte += sprintf(current_byte, " %s", register_name);
                break;
            }
            case (CommandCode::HLT_ID):
                current_byte += sprintf(current_byte, "%s", HLT);
                quit_cycle_flag = true;
                break;
            case (CommandCode::UNK_ID):
                // fall through
            default:
                error = AsmErrors::UNKNOWN_WORD;
        }
        RETURN_IF_ASMERROR(error);

        current_byte += sprintf(current_byte, "\n");

        if (quit_cycle_flag)
            break;
    }

    PrintBuf(out_stream, asm_buf, current_byte - asm_buf);

    free(asm_buf);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

//------------------------------------------------------------------

static AsmErrors ReadPushTarget(const int64_t* byte_buf, size_t* position, char* val)
{
    assert(byte_buf);
    assert(position);
    assert(val);

    AsmErrors error = AsmErrors::NONE;

    PushInfo push = {};
    push.reg = byte_buf[(*position)++];
    push.val = byte_buf[(*position)++];

    if (push.reg == false)
    {
        int printed = sprintf(val, "%lld", push.val);
        if (printed == 0)
            return AsmErrors::PRINT_VALUE;
    }
    else
    {
        error = TranslateByteToRegister((RegisterCode) push.val, val);
        if (error != AsmErrors::NONE)
            return error;
    }

    return error;
}
