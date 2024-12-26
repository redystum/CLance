#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void error(int code, char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\nAborting\n");
    exit(code);
}

void warning(char* format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
    fflush(stderr);
}

void info(char* format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stdout, "INFO: ");
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

void debug(char* file, int line, char* format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stdout, "DEBUG [%s:%d]: ", file, line);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
    fflush(stdout);
}

#ifdef IMPLEMENT_STRING_SLICE_ORIGNAL
void ut_string_slice_original(ut_string_slice_t* str_slice, char** str)
{
    *str = malloc(sizeof(char) * (str_slice->len) + 1);
    if (*str == NULL) {
        error(1, "Error allocating memory for string slice");
    }

    memcpy(*str, str_slice->str, sizeof(char) * (str_slice->len));
    (*str)[str_slice->len] = '\0';
}
#endif

#ifdef IMPLEMENT_READ_FILE
int ut_read_file(char* file_name, char** buffer)
{
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc(length + 1);
    if (*buffer == NULL) {
        fclose(file);
        return -1;
    }

    fread(*buffer, 1, length, file);
    fclose(file);
    (*buffer)[length] = '\0';

    return length;
}
#endif