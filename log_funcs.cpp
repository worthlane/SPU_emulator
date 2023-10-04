#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <strings.h>

#include "log_funcs.h"

static FILE* __LOG_STREAM__ = stderr;

static const char* EXTENSION = ".log";

void OpenLogFile(const char* FILE_NAME)
{
    char* file_name = strndup(FILE_NAME, MAX_FILE_NAME_LEN);

    __LOG_STREAM__ = fopen(strncat(file_name, EXTENSION, MAX_FILE_NAME_LEN + sizeof(EXTENSION)), "a");

    if (__LOG_STREAM__ == nullptr)
        __LOG_STREAM__ =  stderr;

    time_t now = 0;
    time(&now);

    fprintf(__LOG_STREAM__, "\n*********************************************************************\n"
                              "=========================== PROGRAM START ===========================\n"
                              "*********************************************************************\n"
                              "RUNNED AT %s\n", ctime(&now));

    #if CANARY_PROTECT
        fprintf(__LOG_STREAM__, "[CANARY PROTECT ON]\n");
    #endif

    #if HASH_PROTECT
        fprintf(__LOG_STREAM__, "[HASH PROTECT ON]\n");
    #endif

    fputc('\n', __LOG_STREAM__);

    atexit(CloseLogFile);
    free(file_name);
}

//-----------------------------------------------------------------------------------------------------

void CloseLogFile()
{
    fprintf(__LOG_STREAM__, "*********************************************************************\n"
                            "============================ PROGRAM END ============================\n"
                            "*********************************************************************\n");
    fclose(__LOG_STREAM__);
}

//-----------------------------------------------------------------------------------------------------

int LogDump(dump_f dump_func, const void* stk, const char* func, const char* file, const int line)
{
    assert(dump_func);
    assert(stk);

    return dump_func(__LOG_STREAM__, stk, func, file, line);
}

//-----------------------------------------------------------------------------------------------------

int PrintLog (const char *format, ...)
{
  va_list arg;
  int done;

  va_start (arg, format);
  done = vfprintf(__LOG_STREAM__, format, arg);
  va_end (arg);

  return done;
}

