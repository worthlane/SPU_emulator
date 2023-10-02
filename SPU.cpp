#include <strings.h>
#include <stdio.h>

#include "SPU.h"
#include "errors.h"
#include "assembler.h"


static const int MULTIPLIER   = 1000;
static const int MAX_FILE_LEN = 100;

//-------------------------------------------------------------

int SPUCtor(spu_t* spu_info, Stack_t* stk, const char* file_name)
{
    int error = (int) ERRORS::NONE;

    error = StackCtor(stk);
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
    spu_info->stack     = stk;

    return error;
}

//-------------------------------------------------------------

int SPUDtor(spu_t* spu_info)
{
    int error = (int) ERRORS::NONE;

    error = StackDtor((spu_info->stack));
    RETURN_IF_ERROR(error);

    if (spu_info->fp != stdin)
        fclose(spu_info->fp);

    spu_info->file_name = "";
    spu_info->fp        = nullptr;

    return error;
}

//-------------------------------------------------------------

int RunSPU(spu_t* spu_info)
{
    int error = (int) ERRORS::NONE;
    char command[MAX_COMMAND_LEN] = "";
    CommandCode command_code = CommandCode::hlt;

    while (true)
    {
        fscanf(spu_info->fp, "%s", command);

        command_code = HandleCommand(command);

        switch (command_code)
        {
            case (CommandCode::push):
                error = Push(spu_info);
                break;
            case (CommandCode::in):
                printf("in\n");
                break;
            case (CommandCode::out):
                error = Output(spu_info);
                break;
            case (CommandCode::sub):
                error = Substract(spu_info);
                break;
            case (CommandCode::add):
                error = Add(spu_info);
                break;
            case (CommandCode::mul):
                error = Multiply(spu_info);
                break;
            case (CommandCode::div):
                error = Divide(spu_info);
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
            default:
                return (int) ERRORS::UNKNOWN; // TODO error

        }
        RETURN_IF_ERROR(error);
    }
}

//-------------------------------------------------------------

int Push(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;
    elem_t val = POISON;

    size_t val_read = fscanf(spu_info->fp, PRINT_ELEM_T, &val);
    ClearInput(spu_info->fp);

    if (val_read == 0)
        return (int) ERRORS::UNKNOWN; // TODO add error

    error = StackPush((spu_info->stack), val);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int Output(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val = POISON;

    error = StackPop((spu_info->stack), &val);
    RETURN_IF_ERROR(error);

    printf(PRINT_ELEM_T "\n", val);

    return error;
}

//-------------------------------------------------------------

int Substract(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop((spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop((spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 - val1;

    error = StackPush((spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int Add(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop((spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop((spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 + val1;

    error = StackPush((spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int Multiply(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop((spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop((spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 * val1;

    error = StackPush((spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int Divide(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val1 = POISON;
    elem_t val2 = POISON;

    error = StackPop((spu_info->stack), &val1);
    RETURN_IF_ERROR(error);
    error = StackPop((spu_info->stack), &val2);
    RETURN_IF_ERROR(error);

    elem_t result = val2 / val1;

    error = StackPush((spu_info->stack), result);
    RETURN_IF_ERROR(error);

    return error;
}

