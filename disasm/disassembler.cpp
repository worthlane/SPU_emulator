#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "disassembler.h"
#include "../common/log_funcs.h"

static void ClearInput(FILE* fp);

static AsmErrors PrintCmdArguments(const int id, const int64_t* byte_buf,
                                   size_t* position, char** current_byte);

static AsmErrors PrintArgumentsPUSH(const int64_t* byte_buf, size_t* position, char** current_byte);
static AsmErrors PrintArgumentsPOP(const int64_t* byte_buf, size_t* position, char** current_byte);

// --------------------------------------------------------------------------------

#define DEF_CMD(name, id, have_args, ...)                                           \
        case (CommandCode::ID_##name):                                              \
        {                                                                           \
            current_byte += sprintf(current_byte, "%s", name);                      \
                                                                                    \
            if (have_args)                                                          \
            {                                                                       \
                error = PrintCmdArguments(id, byte_buf, &position, &current_byte);  \
                RETURN_IF_ASMERROR(error);                                          \
            }                                                                       \
            if (id == (int) CommandCode::ID_HLT)                                    \
                quit_cycle_flag = true;                                             \
                                                                                    \
            break;                                                                  \
        }


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
    // ===================================================== мб в функцию сунуть пока хз как

    // ================================= ASM BUFFER INIT
    char* asm_buf      = (char*) calloc(MAX_ASM_CODE_LEN, sizeof(char));
    char* current_byte = asm_buf;
    if (asm_buf == nullptr)
        error = AsmErrors::ALLOCATE_MEM;
    RETURN_IF_ASMERROR(error);
    // ================================================= и это тоже

    error = VerifySignature(byte_buf, &position, SIGNATURE, ASM_VER);
    RETURN_IF_ASMERROR(error);

    CommandCode cmd_code = CommandCode::ID_HLT;

    while(true)
    {
        cmd_code = (CommandCode) byte_buf[position++];
        bool  quit_cycle_flag = false;

        switch (cmd_code)
        {
            #include "../common/commands.h"

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

#undef DEF_CMD

//------------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

//------------------------------------------------------------------

static AsmErrors PrintArgumentsPUSH(const int64_t* byte_buf, size_t* position, char** current_byte)
{
    assert(byte_buf);
    assert(position);
    assert(current_byte);

    AsmErrors error = AsmErrors::NONE;

    PushInfo push = {};
    push.reg = byte_buf[(*position)++];
    push.val = byte_buf[(*position)++];

    char val[MAX_WORD_LEN] = "";

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

    *current_byte += sprintf(*current_byte, " %s", val);

    return error;
}

//------------------------------------------------------------------

static AsmErrors PrintArgumentsPOP(const int64_t* byte_buf, size_t* position, char** current_byte)
{
    assert(byte_buf);
    assert(position);
    assert(current_byte);

    AsmErrors error = AsmErrors::NONE;

    RegisterCode reg = (RegisterCode) byte_buf[(*position)++];
    char register_name[MAX_REG_LEN] = "";

    error = TranslateByteToRegister(reg, register_name);
    if (error != AsmErrors::NONE)
        return error;

    *current_byte += sprintf(*current_byte, " %s", register_name);

    return error;
}

//------------------------------------------------------------------

static AsmErrors PrintCmdArguments(const int id, const int64_t* byte_buf,
                                   size_t* position, char** current_byte)
{
    assert(byte_buf);
    assert(position);
    assert(current_byte);

    if (id == (int) CommandCode::ID_PUSH)
        return PrintArgumentsPUSH(byte_buf, position, current_byte);
    else if (id == (int) CommandCode::ID_POP)
        return PrintArgumentsPOP(byte_buf, position, current_byte);

    return AsmErrors::NONE;
}
