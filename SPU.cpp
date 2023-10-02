#include "SPU.h"
#include "errors.h"
#include "assembler.h"


static const int MULTIPLIER = 1000;

//-------------------------------------------------------------

int SPUCtor(spu_t* spu_info)
{
    int error = (int) ERRORS::NONE;

    error = StackCtor(&(spu_info->stack));
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int SPUDtor(spu_t* spu_info)
{
    int error = (int) ERRORS::NONE;

    error = StackDtor(&(spu_info->stack));
    RETURN_IF_ERROR(error);

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
        scanf("%s", command);

        command_code = HandleCommand(command);

        switch (command_code)
        {
            case (CommandCode::push):
                error = Push(spu_info);
                RETURN_IF_ERROR(error);
                break;
            case (CommandCode::in):
                printf("in\n");
                break;
            case (CommandCode::out):
                error = Output(spu_info);
                RETURN_IF_ERROR(error);
                break;
            case (CommandCode::sub):
                printf("sub\n");
                break;
            case (CommandCode::add):
                printf("add\n");
                break;
            case (CommandCode::mul):
                printf("mul\n");
                break;
            case (CommandCode::div):
                printf("div\n");
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
    }
}

//-------------------------------------------------------------

int Push(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;
    elem_t val = POISON;

    size_t val_read = scanf(PRINT_ELEM_T, &val);
    ClearInput(stdin);

    if (val_read == 0)
        return (int) ERRORS::UNKNOWN; // TODO add error

    error = StackPush(&(spu_info->stack), val);
    RETURN_IF_ERROR(error);

    return error;
}

//-------------------------------------------------------------

int Output(spu_t* spu_info)
{
    int error  = (int) ERRORS::NONE;

    elem_t val = POISON;

    error = StackPop(&(spu_info->stack), &val);
    RETURN_IF_ERROR(error);

    printf(PRINT_ELEM_T "\n", val);

    return error;
}
