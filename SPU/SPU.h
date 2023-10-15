#ifndef __SPU_H_
#define __SPU_H_

#include <stdio.h>

#include "stack.h"
#include "../common/commands.h"
#include "../common/input_and_output.h"

enum class SPUErrors
{
    NONE = 0,

    USER_QUIT,

    WRONG_SIGNATURE,

    UNKNOWN_REGISTER,
    EMPTY_REGISTER,
    UNKNOWN_PUSH_MODE,

    PUSH_ERROR,
    POP_ERROR,

    INVALID_STACK,
    NO_FILE_NAME,
    NO_FILE_POINTER,
    NO_BYTE_BUFFER,
    INVALID_BYTE_POSITION,
    NO_REGISTERS_ARRAY,

    UNKNOWN_COMMAND,

    DESTRUCTED
};

struct SPUInfo
{
    Stack_t     stack;

    const char* file_name;
    FILE*       fp;
    int64_t*    byte_buf;
    size_t      position;

    elem_t      registers[REG_AMT];

    SPUErrors   status;
};

static const int SPU_VER = 2;

typedef struct SPUInfo spu_t;

#ifdef RETURN_IF_SPUERROR
#undef RETURN_IF_SPUERROR

#endif
#define RETURN_IF_SPUERROR(error)              do                                                       \
                                            {                                                           \
                                                if ((error) != SPUErrors::NONE)                         \
                                                {                                                       \
                                                    return error;                                       \
                                                }                                                       \
                                            } while(0)

static const char* DEFAULT_IN  = "/Users/amigo/Documents/GitHub/SPU_emulator/assets/byte_code.bin";

ERRORS SPUCtor(ErrorInfo* error, spu_t* spu_info, const char* file_name = DEFAULT_IN);
ERRORS SPUDtor(ErrorInfo* error, spu_t* spu_info);
SPUErrors RunSPU(spu_t* spu_info);

SPUErrors SPUVerify(spu_t* spu, const char* func, const char* file, const int line);
int SPUDump(FILE* fp, const void* stk, const char* func, const char* file, const int line);

#ifdef CHECK_SPU
#undef CHECK_SPU

#endif
#define CHECK_SPU(spu)      do                                                              \
                            {                                                               \
                                SPUVerify((spu), __func__, __FILE__, __LINE__)              \
                                if (spu->status != SPUErrors::NONE)                         \
                                {                                                           \
                                    LogDump(SPUDump, (spu), __func__, __FILE__, __LINE);    \
                                    return spu->status;                                     \
                                }                                                           \
                            } while(0)

#ifdef DUMP_SPU
#undef DUMP_SPU

#endif
#define DUMP_SPU(spu)       do                                                              \
                            {                                                               \
                                LogDump(SPUDump, (spu), __func__, __FILE__, __LINE__);      \
                            } while(0)


#endif
