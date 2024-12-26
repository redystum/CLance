#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "utils.h"
#include "lexer.h"

void replace(char* str, char* orig, char* rep);
bool starts_with(const char* a, const char* b);
ssize_t find_str(const char* str, const char* substr);

int main(int argc, char* argv[])
{
    struct gengetopt_args_info args;

    if (cmdline_parser(argc, argv, &args) != 0) {
        ERROR(1, "Error parsing command line");
        return 1;
    }

    char* input = args.input_arg;
    char* output_arg = args.output_arg;
    bool accept = args.accept_flag;

    // check if ./out exists if not create it
    if (access("./out", F_OK) == -1) {
        if (mkdir("./out", 0777) == -1) {
            ERROR(1, "Error creating output directory");
        }
    }

    char* output = malloc(strlen(output_arg) + 5);
    sprintf(output, "./out/%s", output_arg);

    if (access(output, F_OK) != -1) {
        printf("Output file already exists. Overwrite? (y/N)\n");
        if (accept) {
            printf("y\n");
        } else {
            char c;
            scanf(" %c", &c);
            if (c != 'y') {
                printf("Exiting...\n");
                cmdline_parser_free(&args);
                return 0;
            }
        }
    }

    char* buffer = NULL;

    int length = 0;
    if ((length = ut_read_file(input, &buffer)) == -1) {
        ERROR(1, "Error reading input file");
    }
    printf("Read %d bytes from %s\n", length, input);

    lexer_tokenize(buffer, length);

    

    cmdline_parser_free(&args);
    return 0;
}
