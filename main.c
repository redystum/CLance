#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

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

    ut_dynamic_array_t tokens;
    ut_array_init(&tokens, sizeof(struct token));

    lexer_tokenize(buffer, length, &tokens);

    for (unsigned int i = 0; i < tokens.len; i++) {
        struct token* t = ut_array_get(&tokens, i);
        print_token(t);
    }

    struct parser p;
    struct program_node program;

    ut_dynamic_array_t types_dict;
    ut_array_init(&types_dict, sizeof(struct type_dict));

    parser_init(tokens, types_dict, &p);

    parse_program(&p, &program);
    printf("Parsed program with %d instructions\n", program.instructions.len);
    

    cmdline_parser_free(&args);
    return 0;
}
