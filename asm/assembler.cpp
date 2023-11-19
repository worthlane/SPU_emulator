#include <strings.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "assembler.h"
#include "../common/log_funcs.h"

static AsmErrors TranslateTextToByteCode(code_t* byte_buf, size_t* position, Storage* text_info, label_t* labels);
static AsmErrors UpdateCurrByte(char* input_ptr, size_t* curr_byte);

// ::::::::::::: LABELS FUNCS :::::::::::::

static void InitLabelsArray(label_t* labels, size_t size);
static AsmErrors ReadLabelsFromText(label_t* labels, size_t size, Storage* text_info);
static int FindLabelInArray(const label_t* labels, const char* label_name);

// ::::::::::::::::::::::::::::::::::::::::

// ============ ARGUMENTS FUNCS ============

static AsmErrors HandleCmdArguments(const int id, code_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len, label_t* labels);

static AsmErrors GetLabelOrIntArgument(code_t* byte_buf, size_t* position,
                                  char* line_ptr, size_t* cmd_len, label_t* labels);
static AsmErrors GetRegOrIntArgument(code_t* byte_buf, size_t* position,
                                char* line_ptr, size_t* cmd_len);
static AsmErrors GetRegArgument(code_t* byte_buf, size_t* position,
                                char* line_ptr, size_t* cmd_len);


// =========================================

static RegisterCode GetRegister(char* line_ptr, size_t* cmd_len);

static inline void PrintBytesInTXT(FILE* out_stream, code_t* byte_buf, size_t byte_amt);
static inline void PrintBytesInBIN(const void* buf, size_t size,
                                   size_t amt, FILE* out_stream);

static inline void AddValueInByteCode(code_t* byte_buf, size_t* position, const int* val);
static inline void LogPrintIntInBytes(const int* value);

// -----------------------------------------------------------------------------------

AsmErrors Assembly(FILE* out_stream, FILE* out_bin_stream, Storage* info)
{

    AsmErrors error = AsmErrors::NONE;

    size_t  position = 0;
    code_t* byte_buf = (code_t*) calloc(MAX_BYTE_CODE_LEN, sizeof(code_t));
    if (byte_buf == nullptr)
        error = AsmErrors::ALLOCATE_MEM;
    RETURN_IF_ASMERROR(error);

    label_t labels[MAX_LABELS_AMT] = {};
    InitLabelsArray(labels, MAX_LABELS_AMT);

    error = ReadLabelsFromText(labels, MAX_LABELS_AMT, info);
    RETURN_IF_ASMERROR(error);

    error = TranslateTextToByteCode(byte_buf, &position, info, labels);
    RETURN_IF_ASMERROR(error);

    // добавить фри лэйбелс эррей

    PrintBytesInBIN(byte_buf, sizeof(*byte_buf), position, out_bin_stream);
    PrintBytesInTXT(out_stream, byte_buf, position);

    free(byte_buf);

    return AsmErrors::NONE;
}

//-----------------------------------------------------------------

#define DEF_CMD(name, id, arg_type, byte_size,  ...)                                        \
                if (!strncasecmp(command, #name, MAX_COMMAND_LEN))                          \
                {                                                                           \
                    byte_buf[(*position)++] = (code_t) CommandCode::ID_##name;              \
                                                                                            \
                    LogPrintIntInBytes(&byte_buf[*position - 1]);                           \
                                                                                            \
                    if (arg_type != ArgumentType::NONE)                                     \
                    {                                                                       \
                        error = HandleCmdArguments((int) arg_type, byte_buf, position,      \
                                                   text_info->lines[line].string, &cmd_len, \
                                                   labels);                                 \
                        RETURN_IF_ASMERROR(error);                                          \
                    }                                                                       \
                }                                                                           \
                else

static AsmErrors TranslateTextToByteCode(code_t* byte_buf, size_t* position, Storage* text_info, label_t* labels)
{
    assert(labels);
    assert(byte_buf);
    assert(position);
    assert(text_info);

    AsmErrors error = AsmErrors::NONE;

    AddSignature(byte_buf, position);

    PrintLog("Assemble result: \n"
             "{\n"
             "//SIGNATURE//\n");

    for (size_t line = 0; line < text_info->line_amt; line++)
    {
        if (text_info->lines[line].string[0] == ':')
            continue;

        char command[MAX_COMMAND_LEN] = "";

        size_t cmd_len = 0;
        sscanf(text_info->lines[line].string, "%s%n", command, &cmd_len);

        if (!strncasecmp(command, "", MAX_COMMAND_LEN))
            continue;

        PrintLog("%-4d | ", *(position) * sizeof(int));

        #include "../common/commands.h"

        /*else*/ return AsmErrors::UNKNOWN_WORD;

        error = SyntaxCheckRemainingString(text_info->lines[line].string + cmd_len);
        if (error != AsmErrors::NONE)
            return error;

        PrintLog("\n");
    }

    PrintLog("}\n");

    return error;
}

#undef DEF_CMD

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

static AsmErrors HandleCmdArguments(const int arg_type, code_t* byte_buf, size_t* position,
                                    char* line_ptr, size_t* cmd_len, label_t* labels)
{
    assert(labels);
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    if (arg_type == (int) ArgumentType::REG_OR_INT)
        return GetRegOrIntArgument(byte_buf, position, line_ptr, cmd_len); // можно ли так писать?
    else if (arg_type == (int) ArgumentType::REG)
        return GetRegArgument(byte_buf, position, line_ptr, cmd_len);
    else if (arg_type == (int) ArgumentType::LABEL_OR_INT)
        return GetLabelOrIntArgument(byte_buf, position, line_ptr, cmd_len, labels);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors GetLabelOrIntArgument(code_t* byte_buf, size_t* position,
                                       char* line_ptr, size_t* cmd_len, label_t* labels)
{
    assert(labels);
    assert(byte_buf);
    assert(position);
    assert(cmd_len);
    assert(line_ptr);

    size_t read_symbols = 0;
    int    value        = 0;

    char label_name[MAX_LABEL_NAME] = "";
    int read = sscanf(line_ptr + *cmd_len, " :%s%n", label_name, &read_symbols);

    if (read == 0)
    {
        sscanf(line_ptr + *cmd_len, "%ld%n", &value, &read_symbols);
    }
    else
    {
        int label_id = FindLabelInArray(labels, label_name);
        if (label_id == -1)
            return AsmErrors::UNKNOWN_LABEL;

        value = labels[label_id].jmp_byte;
    }

    *cmd_len += read_symbols;
    AddValueInByteCode(byte_buf, position, &value);

    return AsmErrors::NONE;
}

//------------------------------------------------------------------

static AsmErrors GetRegOrIntArgument(code_t* byte_buf, size_t* position,
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

static AsmErrors GetRegArgument(code_t* byte_buf, size_t* position,
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

//------------------------------------------------------------------

static void InitLabelsArray(label_t* labels, size_t size)
{
    assert(labels);
    assert(size <= MAX_LABELS_AMT);

    for (size_t i = 0; i < size; i++)
    {
        labels[i].jmp_byte   = -1;
        labels[i].label_name = nullptr;
    }
}

//------------------------------------------------------------------

static AsmErrors ReadLabelsFromText(label_t* labels, size_t size, Storage* text_info)
{
    assert(labels);
    assert(text_info);

    AsmErrors error = AsmErrors::NONE;

    size_t labels_read = 0;
    size_t curr_byte   = BYTE_AFTER_SIGNATURE;

    for (size_t line = 0; line < text_info->line_amt; line++)
    {
        if (text_info->lines[line].string[0] != ':')
        {
            error = UpdateCurrByte(text_info->lines[line].string, &curr_byte);
            if (error != AsmErrors::NONE)
                return error;

            continue;
        }

        if (labels_read >= size)
            return AsmErrors::TOO_MANY_LABELS;

        char   label_name[MAX_LABEL_NAME] = "";
        sscanf(text_info->lines[line].string, ":%s", label_name);

        labels[labels_read].jmp_byte   = curr_byte;
        labels[labels_read].label_name = strdup(label_name);

        PrintLog("GOT LABEL \"%s\" ON BYTE %d\n", labels[labels_read].label_name, labels[labels_read].jmp_byte);

        labels_read++;
    }

    return error;
}

//------------------------------------------------------------------

#define DEF_CMD(name, id, arg_type, byte_size, ...)                                  \
                if (!strncasecmp(command, #name, MAX_COMMAND_LEN))                          \
                {                                                                           \
                    *curr_byte += byte_size;                                                \
                }                                                                           \
                else

static AsmErrors UpdateCurrByte(char* input_ptr, size_t* curr_byte)
{
    assert(input_ptr);
    assert(curr_byte);

    char command[MAX_COMMAND_LEN] = "";

    sscanf(input_ptr, "%s", command);

    if (!strncasecmp(command, "", MAX_COMMAND_LEN))
        return AsmErrors::NONE;

    #include "../common/commands.h"
    /*else*/ return AsmErrors::UNKNOWN_WORD;

    return AsmErrors::NONE;
}

#undef DEF_CMD

//------------------------------------------------------------------

static int FindLabelInArray(const label_t* labels, const char* label_name)
{
    assert(labels);
    assert(label_name);

    for (int i = 0; i < MAX_LABELS_AMT; i++)
    {
        if (!strncmp(label_name, labels[i].label_name, MAX_LABEL_NAME))
            return i;
    }

    return -1;
}



