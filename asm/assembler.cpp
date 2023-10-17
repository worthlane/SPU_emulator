#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "assembler.h"
#include "../common/log_funcs.h"

// ============ ARGUMENTS FUNCS ============

static AsmErrors HandleCmdArguments(const int id, int64_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len);

static AsmErrors HandleArgumentsPUSH(int64_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len);
static AsmErrors HandleArgumentsPOP(int64_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len);

// =========================================

static RegisterCode GetRegister(char* line_ptr, size_t* cmd_len);

static inline void PrintBytesInTXT(FILE* out_stream, int64_t* byte_buf, size_t byte_amt);
static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream);

// -----------------------------------------------------------------------------------

#define DEF_CMD(name, id, have_args, ...)                                                   \
                if (!strncmp(command, name, MAX_COMMAND_LEN))                               \
                {                                                                           \
                    byte_buf[position++] = (int64_t) CommandCode::ID_##name;                \
                    if (have_args)                                                          \
                    {                                                                       \
                        error = HandleCmdArguments(id, byte_buf, &position,                 \
                                                   info->lines[line].string, &cmd_len);     \
                        RETURN_IF_ASMERROR(error);                                          \
                    }                                                                       \
                    if (id == (int) CommandCode::ID_HLT)                                    \
                        break;                                                              \
                }                                                                           \
                else

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
        size_t cmd_len = 0;
        sscanf(info->lines[line].string, "%s%n", command, &cmd_len);

        #include "../common/commands.h"

        /*else*/ error = AsmErrors::UNKNOWN_WORD;

        RETURN_IF_ASMERROR(error);

        error = SyntaxCheckRemainingString(info->lines[line].string + cmd_len);
        RETURN_IF_ASMERROR(error);
    }

    PrintBytesInBIN(byte_buf, sizeof(*byte_buf), position, out_bin_stream);
    PrintBytesInTXT(out_stream, byte_buf, position);

    free(byte_buf);

    return AsmErrors::NONE;
}

#undef DEF_CMD();

//------------------------------------------------------------------

static RegisterCode GetRegister(char* line_ptr, size_t* cmd_len)
{
    assert(line_ptr);
    assert(cmd_len);

    char   reg[MAX_REG_LEN] = "";
    size_t read_symbols     = 0;
    sscanf(line_ptr + *cmd_len, "%s%n", reg, &read_symbols);

    *cmd_len += read_symbols;

    RegisterCode reg_code = TranslateRegisterToByte(reg);

    return reg_code;
}

//------------------------------------------------------------------

static AsmErrors HandleCmdArguments(const int id, int64_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    if (id == (int) CommandCode::ID_PUSH)
        return HandleArgumentsPUSH(byte_buf, position, line_ptr, cmd_len); // можно ли так писать?
    else if (id == (int) CommandCode::ID_POP)
        return HandleArgumentsPOP(byte_buf, position, line_ptr, cmd_len);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors HandleArgumentsPUSH(int64_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    elem_t value        = 0;
    size_t read_symbols = 0;
    int    read  = sscanf(line_ptr + *cmd_len, "%ld%n", &value, &read_symbols);

    PushInfo push = {};

    if (read == 0)
    {
        RegisterCode reg_code = GetRegister(line_ptr, cmd_len);
        if (reg_code == RegisterCode::unk)
            return AsmErrors::INVALID_REGISTER;

        push.reg = true;
        push.val = (int) reg_code;
    }
    else
    {
        push.reg = false;
        push.val = value;

        *cmd_len += read_symbols;
    }

    byte_buf[(*position)++] = push.reg;
    byte_buf[(*position)++] = push.val;

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors HandleArgumentsPOP(int64_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    RegisterCode reg_code = GetRegister(line_ptr, cmd_len);
    if (reg_code == RegisterCode::unk)
        return AsmErrors::INVALID_REGISTER;

    byte_buf[(*position)++] = (int64_t) reg_code;

    return AsmErrors::NONE;
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

