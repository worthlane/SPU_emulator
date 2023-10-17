#ifndef DEF_CMD
#define DEF_CMD(...) ;
#endif

DEF_CMD(HLT,  -1,       0)

DEF_CMD(OUT,  1000 - 7, 0)

DEF_CMD(PUSH,  993 - 7, 1)
DEF_CMD(IN,    986 - 7, 1)

DEF_CMD(POP,   979 - 7, 0)

DEF_CMD(SUB,   972 - 7, 0)
DEF_CMD(ADD,   965 - 7, 0)
DEF_CMD(MUL,   958 - 7, 0)
DEF_CMD(DIV,   951 - 7, 0)
DEF_CMD(SQRT,  944 - 7, 0)
DEF_CMD(SIN,   937 - 7, 0)
DEF_CMD(COS,   930 - 7, 0)

DEF_CMD(SPEAK, 923 - 7, 0)
