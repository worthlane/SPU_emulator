#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "assembler.h"
#include "../common/log_funcs.h"

static const size_t MAX_LABELS_AMT = 10;
static const size_t MAX_LABEL_NAME = 100;

// ============ ARGUMENTS FUNCS ============

static AsmErrors HandleCmdArguments(const int id, code_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len);

static AsmErrors HandleArgumentsPUSH(code_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len);
static AsmErrors HandleArgumentsPOP(code_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len);
static AsmErrors HandleArgumentsJMP(code_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len);

// =========================================

static RegisterCode GetRegister(char* line_ptr, size_t* cmd_len);

static inline void PrintBytesInTXT(FILE* out_stream, code_t* byte_buf, size_t byte_amt);
static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream);

static inline void AddValueInByteCode(code_t* byte_buf, size_t* position, const int* val);
static inline void LogPrintIntInBytes(const int* value);

// -----------------------------------------------------------------------------------

#define DEF_CMD(name, id, have_args, ...)                                                   \
                if (!strncasecmp(command, #name, MAX_COMMAND_LEN))                          \
                {                                                                           \
                    byte_buf[position++] = (code_t) CommandCode::ID_##name;                 \
                                                                                            \
                    LogPrintIntInBytes(&byte_buf[position - 1]);                            \
                                                                                            \
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

    code_t* byte_buf  = (code_t*) calloc(MAX_BYTE_CODE_LEN, sizeof(code_t));
    size_t   position = 0;

    if (byte_buf == nullptr)
        error = AsmErrors::ALLOCATE_MEM;

    RETURN_IF_ASMERROR(error);

    AddSignature(byte_buf, &position);

    char command[MAX_COMMAND_LEN] = "";

    PrintLog("Assemble result: \n"
             "{\n"
             "//SIGNATURE//\n");

    for (size_t line = 0; line < info->line_amt; line++)
    {
        PrintLog("%-4d | ", position * sizeof(int));

        size_t cmd_len = 0;
        sscanf(info->lines[line].string, "%s%n", command, &cmd_len);

        #include "../common/commands.h"

        /*else*/ error = AsmErrors::UNKNOWN_WORD;
        RETURN_IF_ASMERROR(error);

        error = SyntaxCheckRemainingString(info->lines[line].string + cmd_len);
        RETURN_IF_ASMERROR(error);

        PrintLog("\n");
    }

    PrintLog("\n}\n");

    PrintBytesInBIN(byte_buf, sizeof(*byte_buf), position, out_bin_stream);
    PrintBytesInTXT(out_stream, byte_buf, position);

    free(byte_buf);

    return AsmErrors::NONE;
}

#undef DEF_CMD;

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

static AsmErrors HandleCmdArguments(const int id, code_t* byte_buf, size_t* position,
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
    else if (id == (int) CommandCode::ID_JMP)
        return HandleArgumentsJMP(byte_buf, position, line_ptr, cmd_len);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors HandleArgumentsJMP(code_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    int value = 0;
    size_t read_symbols = 0;
    sscanf(line_ptr + *cmd_len, "%ld%n", &value, &read_symbols);

    *cmd_len += read_symbols;

    AddValueInByteCode(byte_buf, position, &value);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors HandleArgumentsPUSH(code_t* byte_buf, size_t* position,
                                     char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    elem_t value        = 0;
    size_t read_symbols = 0;
    int    read  = sscanf(line_ptr + *cmd_len, "%ld%n", &value, &read_symbols);

    int reg_flag = false;
    int val      = 0;

    if (read == 0)
    {
        RegisterCode reg_code = GetRegister(line_ptr, cmd_len);
        if (reg_code == RegisterCode::unk)
            return AsmErrors::INVALID_REGISTER;

        reg_flag = true;
        val      = (int) reg_code;
    }
    else
    {
        reg_flag = false;
        val      = value;

        *cmd_len += read_symbols;
    }

    AddValueInByteCode(byte_buf, position, &reg_flag);
    AddValueInByteCode(byte_buf, position, &val);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors HandleArgumentsPOP(code_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len)
{
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    int reg_code = (int) GetRegister(line_ptr, cmd_len);
    if (reg_code == (int) RegisterCode::unk)
        return AsmErrors::INVALID_REGISTER;

    AddValueInByteCode(byte_buf, position, &reg_code);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static inline void PrintBytesInTXT(FILE* out_stream, code_t* byte_buf, size_t byte_amt)
{
    assert(out_stream);
    assert(byte_buf);

    for (size_t pos = 0; pos < byte_amt * 4; pos+=4)
    {
        fprintf(out_stream, "%02X %02X %02X %02X\n",
                             *((unsigned char*) byte_buf + pos),
                                  *((unsigned char*) byte_buf + pos + 1),
                                       *((unsigned char*) byte_buf + pos + 2),
                                            *((unsigned char*) byte_buf + pos + 3));
    }
}

//------------------------------------------------------------------

static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream)
{
    assert(out_stream);
    assert(buf);

    fwrite(buf, size, amt, out_stream);
}

//------------------------------------------------------------------

static inline void AddValueInByteCode(code_t* byte_buf, size_t* position, const int* val)
{
    assert(position);
    assert(byte_buf);

    byte_buf[(*position)++] = *val;

    LogPrintIntInBytes(val);
}

//------------------------------------------------------------------

static inline void LogPrintIntInBytes(const int* val)
{
    assert(val);

    PrintLog("%02X %02X %02X %02X  ",
              *((unsigned char*) val),
                   *((unsigned char*) val + 1),
                        *((unsigned char*) val + 2),
                             *((unsigned char*) val + 3));
}
