#include <strings.h>
#include <stdio.h>
#include <math.h>

#include "SPU.h"
#include "input.h"
#include "../common/errors.h"
#include "../common/commands.h"
#include "../common/input_and_output.h"

static const int MULTIPLIER   = 1000;
static const int MAX_FILE_LEN = 100;

static void PrintRegisters(FILE* fp, const spu_t* spu);

static SPUErrors HandlePushInfo(spu_t* spu, elem_t* val, PushInfo* push);

static int64_t*    SPUByteBufferCtor(FILE* fp, const char* file_name, ErrorInfo* error);
static inline void SPUByteBufferDtor(spu_t* spu);

// =========== COMMANDS =========

static SPUErrors CommandOutput(spu_t* spu);

static SPUErrors CommandPush(spu_t* spu);
static SPUErrors CommandIn(spu_t* spu);
static SPUErrors CommandPop(spu_t* spu);

static void CommandSpeak();

static SPUErrors CommandTwoElemArithm(spu_t* spu, CommandCode operation);
static SPUErrors CommandOneElemArithm(spu_t* spu, CommandCode operation);

// ==============================

// ========= ARITHMETICS =======

static elem_t ALU(spu_t* spu, CommandCode operation, elem_t val1, elem_t val2 = POISON);
// для val мб лучше сделать storage чтобы alu работал красиво и для одного элемента

static inline elem_t Substract(elem_t val1, elem_t val2);
static inline elem_t Add(elem_t val1, elem_t val2);
static inline elem_t Multiply(elem_t val1, elem_t val2);
static inline elem_t Divide(elem_t val1, elem_t val2);
static inline elem_t Sqrt(elem_t val);
static inline elem_t Cos(elem_t val);
static inline elem_t Sin(elem_t val);

// =============================

//-------------------------------------------------------------

ERRORS SPUCtor(ErrorInfo* error, spu_t* spu, const char* file_name)
{
    char* buffer = nullptr;

    // ============================= OPEN FILE

    FILE* fp = fopen(file_name, "rb");

    if (fp == NULL)
    {
        error->data = (char*) file_name;
        return (error->code = ERRORS::OPEN_FILE);
    }

    // ======================================

    error->code = (ERRORS) StackCtor(&(spu->stack));
    RETURN_IF_ERROR(error->code);

    int64_t* byte_buf = SPUByteBufferCtor(fp, file_name, error);
    RETURN_IF_ERROR(error->code);

    spu->byte_buf        = byte_buf;
    spu->position        = 0;
    spu->file_name       = file_name;
    spu->fp              = fp;
    spu->status          = SPUErrors::NONE;

    return error->code;
}

//-------------------------------------------------------------

ERRORS SPUDtor(ErrorInfo* error, spu_t* spu)
{
    error->code = (ERRORS) StackDtor(&(spu->stack));
    RETURN_IF_ERROR(error->code);

    memset(spu->registers, 0, sizeof(register_t) * REG_AMT);

    if (spu->fp != stdin)
        fclose(spu->fp);

    SPUByteBufferDtor(spu);

    spu->file_name       = nullptr;
    spu->fp              = nullptr;
    spu->position        = 0;
    spu->status          = SPUErrors::DESTRUCTED;

    return error->code;
}

//-------------------------------------------------------------

SPUErrors RunSPU(spu_t* spu)
{
    AsmErrors cmd_err = VerifySignature(spu->byte_buf, &(spu->position), SIGNATURE, SPU_VER);
    if (cmd_err != AsmErrors::NONE)
        return (spu->status = SPUErrors::WRONG_SIGNATURE);

    CommandCode command_code = CommandCode::hlt;

    while (true)
    {
        command_code    = (CommandCode) spu->byte_buf[spu->position++];

        bool  quit_cycle_flag = false;
        switch (command_code)
        {
            case (CommandCode::speak):
                CommandSpeak();
                break;
            case (CommandCode::push):
                spu->status = CommandPush(spu);
                break;
            case (CommandCode::in):
                spu->status = CommandIn(spu);
                break;
            case (CommandCode::out):
                spu->status = CommandOutput(spu);
                break;
            case (CommandCode::sub):
                // fall through
            case (CommandCode::add):
                // fall through
            case (CommandCode::mul):
                // fall through
            case (CommandCode::div):
                spu->status = CommandTwoElemArithm(spu, command_code);
                break;
            case (CommandCode::sqrt):
                // fall through
            case (CommandCode::sin):
                // fall through
            case (CommandCode::cos):
                spu->status = CommandOneElemArithm(spu, command_code);
                break;
            case (CommandCode::pop):
                spu->status = CommandPop(spu);
                break;
            case (CommandCode::hlt):
                return SPUErrors::NONE;
            case (CommandCode::unk):
                // fall through
            default:
                return (spu->status = SPUErrors::UNKNOWN_COMMAND);

        }
        RETURN_IF_SPUERROR(spu->status);
    }
    return spu->status;
}

//-------------------------------------------------------------

static SPUErrors CommandPush(spu_t* spu)
{
    PushInfo push = {};

    push.reg = (unsigned int) spu->byte_buf[spu->position++];
    push.val = (elem_t)       spu->byte_buf[spu->position++];

    elem_t val = POISON;

    spu->status = HandlePushInfo(spu, &val, &push);
    RETURN_IF_SPUERROR(spu->status);

    ERRORS push_err = (ERRORS) StackPush(&(spu->stack), val);   // мб с еррорс че то надо менять
    RETURN_IF_NOT_EQUAL(push_err, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return spu->status;
}

//-------------------------------------------------------------

static SPUErrors CommandOutput(spu_t* spu)
{
    elem_t val = POISON;

    ERRORS pop_err = (ERRORS) StackPop(&(spu->stack), &val);
    RETURN_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR);

    printf("%g\n", (double) val / MULTIPLIER);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static inline elem_t Substract(elem_t val1, elem_t val2)
{
    assert(val1 != POISON);
    assert(val2 != POISON);

    return val2 - val1;
}

//-------------------------------------------------------------

static inline elem_t Add(elem_t val1, elem_t val2)
{
    assert(val1 != POISON);
    assert(val2 != POISON);

    return val2 + val1;
}

//-------------------------------------------------------------

static inline elem_t Multiply(elem_t val1, elem_t val2)
{
    assert(val1 != POISON);
    assert(val2 != POISON);

    return (val2 * val1) / MULTIPLIER;
}

//-------------------------------------------------------------

static inline elem_t Divide(elem_t val1, elem_t val2)
{
    assert(val1 != POISON);
    assert(val2 != POISON);

    return (MULTIPLIER * val2) / val1;
}

//-------------------------------------------------------------

static inline elem_t Sqrt(elem_t val)
{
    assert(val != POISON);

    return (elem_t) sqrt((long double) (val * MULTIPLIER));
}

//-------------------------------------------------------------

static inline elem_t Cos(elem_t val)
{
    assert(val != POISON);

    return (elem_t) (cos((long double) val / MULTIPLIER) * MULTIPLIER);
}

//-------------------------------------------------------------

static inline elem_t Sin(elem_t val)
{
    assert(val != POISON);

    return (elem_t) (sin((long double) val / MULTIPLIER) * MULTIPLIER);
}

//-------------------------------------------------------------

static SPUErrors CommandPop(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    RegisterCode reg = (RegisterCode) spu->byte_buf[spu->position++];

    AsmErrors reg_err = VerifyRegister(reg);
    if (reg_err != AsmErrors::NONE)
        return (SPUErrors::UNKNOWN_REGISTER);

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    spu->registers[reg] = val1;

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors HandlePushInfo(spu_t* spu, elem_t* val, PushInfo* push)
{
    assert(spu);
    assert(push);
    assert(val);

    if (push->reg == 1)
    {
        AsmErrors reg_err = VerifyRegister((RegisterCode) push->val);
        if (reg_err != AsmErrors::NONE)
            return (SPUErrors::UNKNOWN_REGISTER);

        if (spu->registers[push->val] == POISON)
            return (SPUErrors::EMPTY_REGISTER);

        *val = spu->registers[push->val];
    }
    else if (push->reg == 0)
    {
        *val = push->val * MULTIPLIER;
    }
    else
        return (SPUErrors::UNKNOWN_PUSH_MODE);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

SPUErrors SPUVerify(spu_t* spu, const char* func, const char* file, const int line)
{
    assert(spu);
    assert(func);
    assert(file);

    if (spu->status != SPUErrors::NONE)
        return spu->status;

    if (!IsStackValid(&(spu->stack), func, file, line))
        spu->status = SPUErrors::INVALID_STACK;

    if (spu->file_name == nullptr)
        spu->status = SPUErrors::NO_FILE_NAME;

    if (spu->fp == nullptr)
        spu->status = SPUErrors::NO_FILE_POINTER;

    if (spu->byte_buf == nullptr)
        spu->status = SPUErrors::NO_BYTE_BUFFER;

    if (spu->position >= MAX_BYTE_CODE_LEN)
        spu->status = SPUErrors::INVALID_BYTE_POSITION;

    if (spu->registers == nullptr)
        spu->status = SPUErrors::NO_REGISTERS_ARRAY;

    return spu->status;
}

//-----------------------------------------------------------------------------------------------------

int SPUDump(FILE* fp, const void* spu_ptr, const char* func, const char* file, const int line)
{
    assert(fp);
    assert(spu_ptr);
    assert(func);
    assert(file);

    const spu_t* spu = (const spu_t*) spu_ptr;

    LOG_START_MOD(func, file, line);

    STACK_DUMP(&(spu->stack));

    fprintf(fp, "SOFTWARE PROCESSING UNIT [%p]\n\n"
                "FILE NAME:    \"%s\"\n"
                "FILE POINTER: [%p]\n"
                "BYTE BUFFER:  [%zu]\n"
                "POSITION:     [%d]\n"
                "STATUS:        %d\n"
                "::::::::::::::::::::\n"
                "REGISTERS:\n\n",
                spu, spu->file_name, spu->fp, spu->byte_buf, spu->position, spu->status);

    PrintRegisters(fp, spu);

    LOG_END();

    return (int) spu->status;
}

//-----------------------------------------------------------------------------------------------------

static void PrintRegisters(FILE* fp, const spu_t* spu)
{
    assert(fp);
    assert(spu);

    fprintf(fp, "%s > " PRINT_ELEM_T "\n"
                "%s > " PRINT_ELEM_T "\n"
                "%s > " PRINT_ELEM_T "\n"
                "%s > " PRINT_ELEM_T "\n",
                RAX, spu->registers[rax],
                RBX, spu->registers[rbx],
                RCX, spu->registers[rcx],
                RDX, spu->registers[rdx]);
}

//-----------------------------------------------------------------------------------------------------

static int64_t* SPUByteBufferCtor(FILE* fp, const char* file_name, ErrorInfo* error)
{
    assert(fp);
    assert(file_name);
    assert(error);

    int64_t* byte_buf = (int64_t*) calloc(MAX_BYTE_CODE_LEN, sizeof(int64_t));
    // мб можно как в дисасм просто массив сделать

    if (byte_buf == nullptr)
    {
        error->code = ERRORS::ALLOCATE_MEMORY;
        return nullptr;
    }

    size_t read = fread(byte_buf, sizeof(int64_t), MAX_BYTE_CODE_LEN, fp);

    if (read == 0)
    {
        error->data = (char*) file_name;
        error->code = ERRORS::READ_FILE;

        return nullptr;
    }

    return byte_buf;
}

//-----------------------------------------------------------------------------------------------------

static inline void SPUByteBufferDtor(spu_t* spu)
{
    assert(spu);

    free(spu->byte_buf);
}

//-----------------------------------------------------------------------------------------------------

static SPUErrors CommandTwoElemArithm(spu_t* spu, CommandCode operation)
{
    assert(spu);

    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = ALU(spu, operation, val1, val2);

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-----------------------------------------------------------------------------------------------------

static SPUErrors CommandOneElemArithm(spu_t* spu, CommandCode operation)
{
    assert(spu);

    ERRORS error = ERRORS::NONE;

    elem_t val = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = ALU(spu, operation, val);

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-----------------------------------------------------------------------------------------------------

static elem_t ALU(spu_t* spu, CommandCode operation, elem_t val1, elem_t val2)
{
    assert(spu);

    elem_t result = POISON;

    switch (operation)
    {
        case (CommandCode::sub):
            result = Substract(val1, val2);
            break;
        case (CommandCode::add):
            result = Add(val1, val2);
            break;
        case (CommandCode::mul):
            result = Multiply(val1, val2);
            break;
        case (CommandCode::div):
            result = Divide(val1, val2);
            break;
        case (CommandCode::sqrt):
            result = Sqrt(val1);
            break;
        case (CommandCode::sin):
            result = Sin(val1);
            break;
        case (CommandCode::cos):
            result = Cos(val1);
            break;
        default:
            return POISON;
    }

    return result;
}

//-----------------------------------------------------------------------------------------------------

static SPUErrors CommandIn(spu_t* spu)
{
    assert(spu);

    ERRORS error = ERRORS::NONE;

    elem_t val = POISON;

    error = GetElement(&val);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::USER_QUIT);

    error = (ERRORS) StackPush(&(spu->stack), val * MULTIPLIER);
    RETURN_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-----------------------------------------------------------------------------------------------------

static void CommandSpeak()
{
    system("say i love counter strike 2");

    PrintLog("speak completed\n");
}
