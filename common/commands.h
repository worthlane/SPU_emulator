#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

DEF_CMD(HLT, 0, ArgumentType::NONE, 4,
{
    quit_cycle_flag = true;
    return (spu->status = SPUErrors::NONE);
})

DEF_CMD(OUT, 1, ArgumentType::NONE, 4,
{
    elem_t val = POISON;

    ERRORS pop_err = (ERRORS) StackPop(&(spu->stack), &val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);

    if (val != POISON)
        printf("%g\n", (double) val / MULTIPLIER);
})

DEF_CMD(PUSH, 2, ArgumentType::NO_LABELS, 8,
{
    long long push = spu->byte_buf[spu->position++];

    elem_t val = POISON;

    if ((arg_params & REG_ARG) != 0)
    {
        AsmErrors reg_err = VerifyRegister((RegisterCode) push);
        if (reg_err != AsmErrors::NONE)
            return (SPUErrors::UNKNOWN_REGISTER);

        if (spu->registers[push] == POISON)
            return (SPUErrors::EMPTY_REGISTER);

        val = spu->registers[push];

        if (arg_params & RAM_ARG != 0)
            val = spu->ram[val];
    }
    else if ((arg_params & NUM_ARG) != 0)
    {
        if ((arg_params & RAM_ARG) != 0)
        {
            if (push >= RAM_SIZE || push < 0)
                return SPUErrors::OUT_RAM_SIZE;

            val = spu->ram[push];
        }
        else
            val = push * MULTIPLIER;
    }

    assert(val != POISON);

    ERRORS push_err = (ERRORS) StackPush(&(spu->stack), val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(push_err, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);
})

DEF_CMD(IN, 3, ArgumentType::NONE, 4,
{
    ERRORS error = ERRORS::NONE;
    elem_t val = POISON;

    error = GetElement(&val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::USER_QUIT, spu->status);

    error = (ERRORS) StackPush(&(spu->stack), val * MULTIPLIER);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);
})

DEF_CMD(POP, 4, ArgumentType::NO_LABELS, 8,
{
    ERRORS error = ERRORS::NONE;

    long long pos = spu->byte_buf[spu->position++];

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);

    if ((arg_params & REG_ARG) != 0)
    {
        AsmErrors reg_err = VerifyRegister((RegisterCode) pos);
        if (reg_err != AsmErrors::NONE)
            return (SPUErrors::UNKNOWN_REGISTER);

        if (arg_params & RAM_ARG != 0)
            spu->ram[spu->registers[pos]] = val1;
        else
            spu->registers[pos] = val1;

    }
    else if ((arg_params & NUM_ARG) != 0 && (arg_params & RAM_ARG) != 0)
    {
        if (pos < 0 || pos >= RAM_SIZE)
            return SPUErrors::OUT_RAM_SIZE;

        spu->ram[pos] = val1;
    }
    else
    {
        return SPUErrors::UNKNOWN_COMMAND;
    }
})

// ====== ARITHMETICS CMDS =======

#ifdef TWO_ELEM_ARITHM
#undef TWO_ELEM_ARITHM

#endif
#define TWO_ELEM_ARITHM(operation)                                                              \
{                                                                                               \
    ERRORS error = ERRORS::NONE;                                                                \
                                                                                                \
    elem_t val1 = POISON;                                                                       \
    elem_t val2 = POISON;                                                                       \
                                                                                                \
    error = (ERRORS) StackPop(&(spu->stack), &val1);                                            \
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);     \
                                                                                                \
    error = (ERRORS) StackPop(&(spu->stack), &val2);                                            \
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);     \
                                                                                                \
    elem_t result = ALU(spu, operation, val1, val2);                                            \
                                                                                                \
    error = (ERRORS) StackPush(&(spu->stack), result);                                          \
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);    \
}

DEF_CMD(SUB, 5, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(ADD, 6, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(MUL, 7, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(DIV, 8, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})

#undef TWO_ELEM_ARITHM

#ifdef ONE_ELEM_ARITHM
#undef ONE_ELEM_ARITHM

#endif
#define ONE_ELEM_ARITHM(operation)                                                              \
{                                                                                               \
    ERRORS error = ERRORS::NONE;                                                                \
                                                                                                \
    elem_t val = POISON;                                                                        \
                                                                                                \
    error = (ERRORS) StackPop(&(spu->stack), &val);                                             \
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);     \
                                                                                                \
    elem_t result = ALU(spu, operation, val);                                                   \
                                                                                                \
    error = (ERRORS) StackPush(&(spu->stack), result);                                          \
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);    \
}

DEF_CMD(SQRT, 9, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})
DEF_CMD(SIN, 10, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})
DEF_CMD(COS, 11, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})

// ================================

DEF_CMD(SPEAK, 12, ArgumentType::NONE, 4,
{
    system("say i love counter strike 2");

    PrintLog("speak completed\n");
})

// ............. JMP ..............

DEF_CMD(JMP, 13, ArgumentType::HAS_LABELS, 8,
{
    code_t start = spu->byte_buf[spu->position++] / sizeof(int);

    spu->position = start;
})

#ifdef MAKE_COND_JMP
#undef MAKE_COND_JMP

#endif
#define MAKE_COND_JMP(name, id, operation)                                                                  \
        DEF_CMD(name, id, ArgumentType::HAS_LABELS, 8,                                                    \
        {                                                                                                   \
            code_t start = spu->byte_buf[spu->position++] / sizeof(int);                                    \
                                                                                                            \
            ERRORS pop_err = ERRORS::NONE;                                                                  \
                                                                                                            \
            elem_t val1 = POISON;                                                                           \
            elem_t val2 = POISON;                                                                           \
                                                                                                            \
            pop_err = (ERRORS) StackPop(&(spu->stack), &val1);                                              \
            UPDATE_SPU_STATUS_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);       \
                                                                                                            \
            pop_err = (ERRORS) StackPop(&(spu->stack), &val2);                                              \
            UPDATE_SPU_STATUS_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);       \
                                                                                                            \
            if (val1 operation val2)                                                                      \
                spu->position = start;                                                                      \
        })

MAKE_COND_JMP(JA, 14, >)
MAKE_COND_JMP(JAE, 15, >=)
MAKE_COND_JMP(JB, 16, <)
MAKE_COND_JMP(JBE, 17, <=)
MAKE_COND_JMP(JNE, 18, !=)
MAKE_COND_JMP(JE, 19, ==)

#undef MAKE_COND_JMP

DEF_CMD(CALL, 20, ArgumentType::HAS_LABELS, 8,
{
    code_t start = spu->byte_buf[spu->position++] / sizeof(int);

    ERRORS push_err = (ERRORS) StackPush(&(spu->returns), spu->position);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(push_err, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);

    spu->position = start;
})

DEF_CMD(RET, 21, ArgumentType::NONE, 4,
{
    elem_t ret = POISON;

    ERRORS pop_err = (ERRORS) StackPop(&(spu->returns), &ret);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);

    spu->position = ret;
})


// .............................................
