#include <strings.h>
#include <stdio.h>
#include <math.h>

#include "SPU.h"
#include "../common/errors.h"
#include "../common/commands.h"
#include "../common/input_and_output.h"

static const int MULTIPLIER   = 1000;
static const int MAX_FILE_LEN = 100;

static void PrintRegisters(FILE* fp, const spu_t* spu);

static SPUErrors HandlePushSPU(spu_t* spu, elem_t* val, PushInfo* push);

// =========== COMMANDS =========

static SPUErrors CommandOutput(spu_t* spu);

static SPUErrors CommandPush(spu_t* spu);
static SPUErrors CommandPop(spu_t* spu);

static SPUErrors CommandSubstract(spu_t* spu);
static SPUErrors CommandAdd(spu_t* spu);
static SPUErrors CommandMultiply(spu_t* spu);
static SPUErrors CommandDivide(spu_t* spu);
static SPUErrors CommandSqrt(spu_t* spu);
static SPUErrors CommandCos(spu_t* spu);
static SPUErrors CommandSin(spu_t* spu);

// ==============================

static void ClearInput(FILE* fp);

//-------------------------------------------------------------

ERRORS SPUCtor(ErrorInfo* error, spu_t* spu, const char* file_name)
{
    char* buffer = nullptr;

    FILE* fp = fopen(file_name, "rb");

    if (fp == NULL)
    {
        error->data = (char*) file_name;
        return (error->code = ERRORS::OPEN_FILE);
    }

    off_t buf_size  = GetFileLength(file_name);

    AllocateBuf(fp, &buffer, buf_size, error);
    RETURN_IF_ERROR(error->code);

    error->code = (ERRORS) StackCtor(&(spu->stack));
    RETURN_IF_ERROR(error->code);

    spu->input_buf       = buffer;
    spu->curr_input_byte = buffer;
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

    if (spu->fp != stdin)
        fclose(spu->fp);

    spu->file_name       = "";
    spu->fp              = nullptr;
    spu->curr_input_byte = nullptr;
    spu->input_buf       = nullptr;
    spu->status          = SPUErrors::DESTRUCTED;

    return error->code;
}

//-------------------------------------------------------------

SPUErrors RunSPU(spu_t* spu)
{
    AsmErrors cmd_err = VerifySignature(spu->curr_input_byte, SIGNATURE, SPU_VER);
    if (cmd_err != AsmErrors::NONE)
        return (spu->status = SPUErrors::WRONG_SIGNATURE);

    spu->curr_input_byte += SIGNATURE_LEN;

    char command[MAX_COMMAND_LEN] = "";
    CommandCode      command_code = CommandCode::hlt;

    while (true)
    {
        long command_id = strtol(spu->curr_input_byte, &(spu->curr_input_byte), 10);
        command_code    = (CommandCode) command_id;

        bool  quit_cycle_flag = false;
        switch (command_code)
        {
            case (CommandCode::push):
                spu->status = CommandPush(spu);
                break;
            case (CommandCode::in):
                printf("in\n");
                break;
            case (CommandCode::out):
                spu->status = CommandOutput(spu);
                break;
            case (CommandCode::sub):
                spu->status = CommandSubstract(spu);
                break;
            case (CommandCode::add):
                spu->status = CommandAdd(spu);
                break;
            case (CommandCode::mul):
                spu->status = CommandMultiply(spu);
                break;
            case (CommandCode::div):
                spu->status = CommandDivide(spu);
                break;
            case (CommandCode::sqrt):
                spu->status = CommandSqrt(spu);
                break;
            case (CommandCode::sin):
                spu->status = CommandSin(spu);
                break;
            case (CommandCode::cos):
                spu->status = CommandCos(spu);
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

    push.reg = (unsigned int) strtol(spu->curr_input_byte, &(spu->curr_input_byte), 10);
    push.val = (elem_t) strtol(spu->curr_input_byte, &(spu->curr_input_byte), 10);

    elem_t val = POISON;

    spu->status = HandlePushSPU(spu, &val, &push);
    RETURN_IF_SPUERROR(spu->status);

    ERRORS push_err = (ERRORS) StackPush(&(spu->stack), val);
    RETURN_CONVERTED_ERROR(push_err, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return spu->status;
}

//-------------------------------------------------------------

static SPUErrors CommandOutput(spu_t* spu)
{
    elem_t val = POISON;

    ERRORS pop_err = (ERRORS) StackPop(&(spu->stack), &val);
    RETURN_CONVERTED_ERROR(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR);

    printf("%g\n", (double) val / MULTIPLIER);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandSubstract(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = val2 - val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandAdd(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = val2 + val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandMultiply(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = (val2 * val1) / MULTIPLIER;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandDivide(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    elem_t result = (MULTIPLIER * val2) / val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandSqrt(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    val1 = (elem_t) sqrt((long double) (val1 * MULTIPLIER));

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandCos(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    val1 = (elem_t) (cos((long double) val1 / MULTIPLIER) * MULTIPLIER);

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandSin(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    val1 = (elem_t) (sin((long double) val1 / MULTIPLIER) * MULTIPLIER);

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::PUSH_ERROR);

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static SPUErrors CommandPop(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    RegisterCode reg = (RegisterCode) strtol(spu->curr_input_byte, &(spu->curr_input_byte), 10);

    AsmErrors reg_err = VerifyRegister(reg);
    if (reg_err != AsmErrors::NONE)
        return (SPUErrors::UNKNOWN_REGISTER);

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_CONVERTED_ERROR(error, ERRORS::NONE, SPUErrors::POP_ERROR);

    spu->registers[reg] = val1;

    return (SPUErrors::NONE);
}

//-------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

//-------------------------------------------------------------

static SPUErrors HandlePushSPU(spu_t* spu, elem_t* val, PushInfo* push)
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

    if (spu->input_buf == nullptr)
        spu->status = SPUErrors::NO_INPUT_BUFFER;

    if (spu->curr_input_byte < spu->input_buf)
        spu->status = SPUErrors::INVALID_BYTE_POINTER;

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
                "FILE NAME: \"%s\"\n"
                "FILE POINTER: [%p]\n"
                "INPUT BUFFER: [%zu]\n"
                "CURRENT BYTE: [%zu]\n"
                "STATUS:        %d\n"
                "::::::::::::::::::::\n"
                "REGISTERS:\n\n",
                spu, spu->file_name, spu->fp, spu->input_buf, spu->curr_input_byte, spu->status);

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
