#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

DEF_CMD(HLT, -1, ArgumentType::NONE, 4,
{
    return (spu->status = SPUErrors::NONE);
})

DEF_CMD(OUT, 1000 - 7, ArgumentType::NONE, 4,
{
    elem_t val = POISON;

    ERRORS pop_err = (ERRORS) StackPop(&(spu->stack), &val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(pop_err, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);

    printf("%g\n", (double) val / MULTIPLIER);
})

DEF_CMD(PUSH, 993 - 7, ArgumentType::REG_OR_INT, 12,
{
    PushInfo push = {};

    push.reg = (unsigned int) spu->byte_buf[spu->position++];
    push.val = (elem_t)       spu->byte_buf[spu->position++];

    elem_t val = POISON;

    spu->status = HandlePushInfo(spu, &val, &push);
    if (spu->status != SPUErrors::NONE)
        break;

    ERRORS push_err = (ERRORS) StackPush(&(spu->stack), val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(push_err, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);
})

DEF_CMD(IN, 986 - 7, ArgumentType::NONE, 4,
{
    ERRORS error = ERRORS::NONE;
    elem_t val = POISON;

    error = GetElement(&val);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::USER_QUIT, spu->status);

    error = (ERRORS) StackPush(&(spu->stack), val * MULTIPLIER);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::PUSH_ERROR, spu->status);
})

DEF_CMD(POP, 979 - 7, ArgumentType::REG, 8,
{
    ERRORS error = ERRORS::NONE;

    RegisterCode reg = (RegisterCode) spu->byte_buf[spu->position++];

    AsmErrors reg_err = VerifyRegister(reg);
    if (reg_err != AsmErrors::NONE)
        spu->status = SPUErrors::UNKNOWN_REGISTER;

    elem_t val1 = POISON;

    error = (ERRORS) StackPop(&(spu->stack), &val1);
    UPDATE_SPU_STATUS_IF_NOT_EQUAL(error, ERRORS::NONE, SPUErrors::POP_ERROR, spu->status);

    spu->registers[reg] = val1;
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

DEF_CMD(SUB, 972 - 7, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(ADD, 965 - 7, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(MUL, 958 - 7, ArgumentType::NONE, 4,
{
    TWO_ELEM_ARITHM(command_code)
})
DEF_CMD(DIV, 951 - 7, ArgumentType::NONE, 4,
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

DEF_CMD(SQRT, 944 - 7, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})
DEF_CMD(SIN, 937 - 7, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})
DEF_CMD(COS, 930 - 7, ArgumentType::NONE, 4,
{
    ONE_ELEM_ARITHM(command_code);
})

// ================================

DEF_CMD(SPEAK, 923 - 7, ArgumentType::NONE, 4,
{
    system("say i love counter strike 2");

    PrintLog("speak completed\n");
})

DEF_CMD(JMP, 916 - 7, ArgumentType::LABEL_OR_INT, 8,
{
    size_t start = spu->byte_buf[spu->position++] / sizeof(int);

    PrintLog("start %d %d\n", start, start * 4);

    spu->position = start;
})
