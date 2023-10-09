#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>

#include "input_and_output.h"
#include "errors.h"

static int FillLineStruct(struct ErrorInfo* error,
                          size_t line_amt, off_t text_len, char* buf, struct LineInfo* lines);
static int AllocateLines(struct LineInfo** lines, char* buf, const off_t text_len,
                         size_t* line_amt, struct ErrorInfo* error);

static int AllocateLines(struct LineInfo** lines, char* buf, const off_t text_len,
                         size_t* line_amt, struct ErrorInfo* error)
{
    for (int i = 0; i < text_len; i++)
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            (*line_amt)++;
        }
    }

    (*line_amt)++;

    *lines = (struct LineInfo* ) calloc(*line_amt, sizeof(struct LineInfo));

    if (*lines ==  NULL)
        return (int) (error->code = ERRORS::ALLOCATE_MEMORY);

    (*lines)[0].string = buf;

    return (int) ERRORS::NONE;
}

//-------------------------------------------------------------------------------------------

int AllocateBuf(FILE* fp, char** buf, const off_t buf_len, struct ErrorInfo* error)
{
    // Add 1 for NUL-terminator --v
    *buf = (char* ) calloc(buf_len + 1, sizeof(char));

    if (!*buf)
        return (int) (error->code = ERRORS::ALLOCATE_MEMORY);

    off_t symbols_read = fread(*buf, sizeof(char), buf_len, fp);

    if (symbols_read != buf_len)
        return (int) (error->code = ERRORS::READ_FILE);

    return (int) ERRORS::NONE;
}

//-------------------------------------------------------------------------------------------

static int FillLineStruct(struct ErrorInfo* error,
                          size_t line_amt, off_t text_len, char* buf, struct LineInfo* lines)
{
    assert(error);
    assert(lines);
    assert(buf);

    size_t line = 1;

    for (off_t i = 0; i < text_len; i++)
    {
        if (buf[i] == '\0')
        {
            lines[line - 1].len  = buf + i - lines[line - 1].string;
            lines[line++].string = buf + i + 1;
        }
    }

    return (int) (error->code = ERRORS::NONE);
}

//-------------------------------------------------------------------------------------------

bool PrintAllLines(FILE* stream, const struct LineInfo* lines,
                   const size_t line_amount, struct ErrorInfo* error)
{
    assert(lines);

    for (size_t line = 0; line < line_amount; line++)
    {
        PrintOneLine(stream, &lines[line], error);
        if (error->code == ERRORS::PRINT_DATA)
            return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------

void PrintOneLine(FILE* stream, const struct LineInfo* line, struct ErrorInfo* error)
{
    assert(line);

    for (int i = 0; (line->string)[i] != '\0' && i < line->len; i++)
    {
        if (fputc((line->string)[i], stream) == EOF)
        {
            error->code = ERRORS::PRINT_DATA;
            return;
        }
    }

    if (fputc('\n', stream) == EOF)
            error->code = ERRORS::PRINT_DATA;
}

//-------------------------------------------------------------------------------------------

off_t GetFileLength(const char* file_name)
{
    assert(file_name);

    struct stat buf;
    stat(file_name, &buf);

    return buf.st_size;
}

//-------------------------------------------------------------------------------------------

int CreateTextStorage(struct Storage* info, struct ErrorInfo* error, const char* FILE_NAME)
{
    assert(info);

    FILE* fp = fopen(FILE_NAME, "rb");

    if (fp == NULL)
    {
        error->data = (char*) FILE_NAME;
        return (int) (error->code = ERRORS::OPEN_FILE);
    }

    off_t buf_size  = GetFileLength(FILE_NAME);
    char* buffer    = nullptr;
    size_t line_amt = 0;
    struct LineInfo* lines = {};

    //===================== CREATING BUFFER

    AllocateBuf(fp, &buffer, buf_size, error);

    if (error->code != ERRORS::NONE)
        return (int) error->code;

    //===================== END OF CREATING BUFFER

    //===================== CREATING LINES STRUCTURES

    AllocateLines(&lines, buffer, buf_size, &line_amt, error);

    if (error->code != ERRORS::NONE)
    {
        info->line_amt = 0;
        return (int) error->code;
    }

    FillLineStruct(error, line_amt, buf_size, buffer, lines);

    if (error->code != ERRORS::NONE)
        return (int) error->code;

    //===================== END OF CREATING LINES STRUCTURES

    fclose(fp);

    info->lines    = lines;
    info->buf      = buffer;
    info->line_amt = line_amt;
    info->text_len = buf_size;

    if (info->lines == NULL)
    {
        info->line_amt = 0;
        return (int) (error->code = ERRORS::ALLOCATE_MEMORY);
    }

    return (int) (error->code = ERRORS::NONE);
}

//-------------------------------------------------------------------------------------------

bool EraseFile(const char* FILE_NAME)
{
    FILE* fp = fopen(FILE_NAME, "wb");

    if (fp == NULL)
        return false;

    fclose(fp);

    return true;
}

//-------------------------------------------------------------------------------------------

void PrintBuf(FILE* stream, const char* buf, const size_t buf_len)
{
    assert(buf);

    for (size_t i = 0; i < buf_len; i++)
    {
        int ch = buf[i];

        if (buf[i] == '\0')
            ch = '\n';

        fputc(ch, stream);
    }

}

//-------------------------------------------------------------------------------------------

bool PrintHeader(FILE* stream, const char* header)
{
    if (fprintf(stream, "%s\n\n", header) == 0)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------

bool PrintSeparator(FILE* stream)
{
    if (fprintf(stream, "------------------------------------------------------------------\n") == 0)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------

FILE* OpenInputFile(const char* input_file, Storage* info, ErrorInfo* error)
{
    FILE* in_stream  = fopen(input_file, "rb");

    if (in_stream == nullptr)
    {
        error->code = ERRORS::OPEN_FILE;
        error->data = (void*) input_file;
        return in_stream;
    }

    CreateTextStorage(info, error, input_file);

    return in_stream;
}

//-------------------------------------------------------------------------------------------

FILE* OpenOutputFile(const char* output_file, ErrorInfo* error)
{
    FILE* out_stream  = fopen(output_file, "w");

    if (out_stream == nullptr)
    {
        error->code = ERRORS::OPEN_FILE;
        error->data = (void*) output_file;
    }

    return out_stream;
}

//-------------------------------------------------------------------------------------------

FILE* OpenBinOutputFile(const char* output_file, ErrorInfo* error)
{
    FILE* out_stream  = fopen(output_file, "wb");

    if (out_stream == nullptr)
    {
        error->code = ERRORS::OPEN_FILE;
        error->data = (void*) output_file;
    }

    return out_stream;
}
