#include <strings.h>
#include <stdio.h>
#include <math.h>

#include "SPU.h"
#include "../errors.h"
#include "../asm/commands.h"

static const int MULTIPLIER   = 1000;
static const int MAX_FILE_LEN = 100;

static int CommandPush(spu_t* spu_info);
static int CommandOutput(spu_t* spu_info);
static int CommandSubstract(spu_t* spu_info);
static int CommandAdd(spu_t* spu_info);
static int CommandMultiply(spu_t* spu_info);
static int CommandDivide(spu_t* spu_info);
static int CommandSqrt(spu_t* spu_info);
static int CommandCos(spu_t* spu_info);
static int CommandSin(spu_t* spu_info);

static void ClearInput(FILE* fp);

//-------------------------------------------------------------

int SPUCtor(spu_t* spu_info, const char* file_name)
{
    int error = (int) ERRORS::NONE;

    error = StackCtor(&(spu_info->stack));
    RETURN_IF_ERROR(error);

    FILE* fp = nullptr;

    if (!strncmp(file_name, "stdin", MAX_FILE_LEN))
        fp = stdin;
    else
    {
        fp = fopen(file_name, "r");
        if (fp == nullptr)
            return (int) ERRORS::OPEN_FILE;
    }

    spu_info->file_name = file_name;
    spu_info->fp        = fp;

    return error;
}

//-------------------------------------------------------------

int SPUDtor(spu_t* spu_info)
{
    int error = (int) ERRORS::NONE;

    error = StackDtor(&(spu_info->stack));
    RETURN_IF_ERROR(error);

    if (spu_info->fp != stdin)
        fclose(spu_info->fp);

    spu_info->file_name = "";
    spu_info->fp        = nullptr;

    return error;
}

//-------------------------------------------------------------

int RunSPU(spu_t* spu_info) // убрать spu_info
{
    // засунуть SPUCtor

    int error   = (int) ERRORS::NONE;
    int command = 0;
    CommandCode command_code = CommandCode::hlt;

    while (true)
    {
        fscanf(spu_info->fp, "%d", &command);

        command_code = (CommandCode) command;

        switch (command_code)
        {
            case (CommandCode::push):
                error = CommandPush(spu_info);
                break;
            case (CommandCode::in):
                printf("in\n");
                break;
            case (CommandCode::out):
                error = CommandOutput(spu_info);
                break;
            case (CommandCode::sub):
                error = CommandSubstract(spu_info);
                break;
            case (CommandCode::add):
                error = CommandAdd(spu_info);
                break;
            case (CommandCode::mul):
                error = CommandMultiply(spu_info);
                break;
            case (CommandCode::div):
                error = CommandDivide(spu_info);
                break;
            case (CommandCode::sqrt):
                printf("sqrt\n");
                break;
            case (CommandCode::sin):
                printf("sin\n");
                break;
            case (CommandCode::cos):
                printf("out\n");
                break;
            case (CommandCode::hlt):
                return (int) ERRORS::NONE;
            case (CommandCode::unk):
                // fall through
            default:
                return (int) ERRORS::SPU_ERROR; // TODO error

        }
        RETURN_IF_ERROR(error);
    }

    // засунуть SPUDtor

    return error;
}

//-------------------------------------------------------------

static int CommandPush(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;
    elem_t val = POISON;

    int val_read = fscanf(spu_info->fp, PRINT_ELEM_T, &val);
    ClearInput(spu_info->fp);

    if (val_read == 0)
        return (int) ERRORS::UNKNOWN; // TODO add error

    error = StackPush(&(spu_info->stack), val * MULTIPLIER);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandOutput(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val = POISON;

    error = StackPop(&(spu_info->stack), &val);
    RETURN_IF_ERROR(error);

    printf("%g\n", (double) val / MULTIPLIER);

    return error;
}

//-------------------------------------------------------------

static int CommandSubstract(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop(&(spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 - val1;

    error = StackPush(&(spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandAdd(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop(&(spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 + val1;

    error = StackPush(&(spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandMultiply(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop(&(spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = (val2 * val1) / MULTIPLIER;

    error = StackPush(&(spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandDivide(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop(&(spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = (MULTIPLIER * val2) / val1;

    error = StackPush(&(spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandSqrt(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);

    val1 = (elem_t) sqrt((long double) (val1 * MULTIPLIER));

    error = StackPush(&(spu_info->stack), val1);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static int CommandCos(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;

    error = StackPop(&(spu_info->stack), &val1);
    RETURN_IF_ERROR(error);

    // TODO val1 = (elem_t) cos((long double) );

    error = StackPush(&(spu_info->stack), val1);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

static void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

