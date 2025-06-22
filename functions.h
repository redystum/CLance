#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define INPUT_INT_HEADER "int input_int(const char *prompt);\n"
#define INPUT_INT "int input_int(const char *prompt) {"\
"    char buffer[100];"\
"    char *endptr;"\
"    int result;"\
"    while (1) {"\
"        printf(\"%s\", prompt);"\
"        if (!fgets(buffer, sizeof(buffer), stdin)) {"\
"            printf(\"Error reading input.\\n\");"\
"            continue;"\
"        }"\
"        buffer[strcspn(buffer, \"\\n\")] = 0;"\
"        errno = 0;"\
"        long value = strtol(buffer, &endptr, 10);"\
"        if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {"\
"            printf(\"Number out of range. Try again.\\n\");"\
"            continue;"\
"        }"\
"        if (endptr == buffer || *endptr != '\\0') {"\
"            printf(\"Invalid input. Please enter a valid integer.\\n\");"\
"            continue;"\
"        }"\
"        result = (int)value;"\
"        break;"\
"    }"\
"    return result;"\
"}"

#define INPUT_INT_CALLER_FMT "input_int(\"%s\");\n"

#define INPUT_STRING_HEADER "char *input_string(const char *prompt);\n"
#define INPUT_STRING "char *ask_string(const char *prompt) {"\
"    char *line = NULL;"\
"    size_t len = 0;"\
"    printf(\"%s\", prompt);"\
"    ssize_t read = getline(&line, &len, stdin);"\
"    if (read == -1) {"\
"        free(line);"\
"        return NULL;"\
"    }"\
"    if (line[read - 1] == '\\n') {"\
"        line[read - 1] = '\\0';"\
"    }"\
"    return line;"\
"}"

#define INPUT_STRING_CALLER_FMT "ask_string(\"%s\");\n"

#endif // FUNCTIONS_H