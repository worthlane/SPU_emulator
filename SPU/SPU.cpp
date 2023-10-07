#include <strings.h>
#include <stdio.h>
#include <math.h>

#include "SPU.h"
#include "../common/errors.h"
#include "../common/commands.h"
#include "../common/input_and_output.h"

static const int MULTIPLIER   = 1000;
static const int MAX_FILE_LEN = 100;

static ERRORS CommandPush(spu_t* spu);
static ERRORS CommandOutput(spu_t* spu);
static ERRORS CommandSubstract(spu_t* spu);
static ERRORS CommandAdd(spu_t* spu);
static ERRORS CommandMultiply(spu_t* spu);
static ERRORS CommandDivide(spu_t* spu);
static ERRORS CommandSqrt(spu_t* spu);
static ERRORS CommandCos(spu_t* spu);
static ERRORS CommandSin(spu_t* spu);

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

    return error->code;
}

//-------------------------------------------------------------

ERRORS RunSPU(ErrorInfo* error, spu_t* spu)
{
    char command[10] = "";
    CommandCode command_code = CommandCode::hlt;

    while (true)
    {
        long command_id = strtol(spu->curr_input_byte, &(spu->curr_input_byte), 10);
        command_code    = (CommandCode) command_id;

        bool  quit_cycle_flag = false;
        switch (command_code)
        {
            case (CommandCode::push):
                error->code = CommandPush(spu);
                break;
            case (CommandCode::in):
                printf("in\n");
                break;
            case (CommandCode::out):
                error->code = CommandOutput(spu);
                break;
            case (CommandCode::sub):
                error->code = CommandSubstract(spu);
                break;
            case (CommandCode::add):
                error->code = CommandAdd(spu);
                break;
            case (CommandCode::mul):
                error->code = CommandMultiply(spu);
                break;
            case (CommandCode::div):
                error->code = CommandDivide(spu);
                break;
            case (CommandCode::sqrt):
                error->code = CommandSqrt(spu);
                break;
            case (CommandCode::sin):
                error->code = CommandSin(spu);
                break;
            case (CommandCode::cos):
                error->code = CommandCos(spu);
                break;
            case (CommandCode::hlt):
                return ERRORS::NONE;
            case (CommandCode::unk):
                // fall through
            default:
                return ERRORS::SPU_ERROR; // TODO error

        }
        RETURN_IF_ERROR(error->code);
    }
    return error->code;
}

//-------------------------------------------------------------

static ERRORS CommandPush(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    double a = strtod(spu->curr_input_byte, &(spu->curr_input_byte));

    elem_t val = (elem_t) a;

    error = (ERRORS) StackPush(&(spu->stack), val * MULTIPLIER);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandOutput(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val);
    RETURN_IF_ERROR(error);

    printf("%g\n", (double) val / MULTIPLIER);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandSubstract(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);
    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 - val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandAdd(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);
    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 + val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandMultiply(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);
    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = (val2 * val1) / MULTIPLIER;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandDivide(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);
    error = (ERRORS) StackPop(&(spu->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = (MULTIPLIER * val2) / val1;

    error = (ERRORS) StackPush(&(spu->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandSqrt(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);

    val1 = (elem_t) sqrt((long double) (val1 * MULTIPLIER));

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandCos(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);

    val1 = (elem_t) (cos((long double) val1 / MULTIPLIER) * MULTIPLIER);

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static ERRORS CommandSin(spu_t* spu)
{
    ERRORS error = ERRORS::NONE;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    RETURN_IF_ERROR(error);

    val1 = (elem_t) (sin((long double) val1 / MULTIPLIER) * MULTIPLIER);

    error = (ERRORS) StackPush(&(spu->stack), val1);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

