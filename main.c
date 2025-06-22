#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "args.h"
#include "assembler.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

void replace(char *str, char *orig, char *rep);
bool starts_with(const char *a, const char *b);
ssize_t find_str(const char *str, const char *substr);

int verbose_enabled = 0;

int main(int argc, char *argv[]) {
	struct gengetopt_args_info args;

	if (cmdline_parser(argc, argv, &args) != 0) {
		ERROR(1, "Error parsing command line");
		return 1;
	}

	verbose_enabled = args.verbose_flag;

#ifdef DEBUG_ENABLED
	verbose_enabled = 1;
#endif

	char *input = args.input_arg;
	char *output_arg = args.output_arg;
	bool accept = args.accept_flag;
	bool indent = args.format_flag;

	// check if ./out exists if not create it
	if (access("./out", F_OK) == -1) {
		if (mkdir("./out", 0777) == -1) {
			ERROR(1, "Error creating output directory");
		}
	}

	char *output = malloc(strlen(output_arg) + 6);
	if (output == NULL) {
		ERROR(1, "Failed to allocate memory for output path");
	}
	sprintf(output, "./out/%s", output_arg);

	if (access(output, F_OK) != -1) {
		printf("Output file already exists. Overwrite? (y/N)\n");
		if (accept) {
			printf("y\n");
		} else {
			char c;
			scanf(" %c", &c);
			if (c != 'y') {
				INFO("Exiting...\n");
				cmdline_parser_free(&args);
				free(output);
				return 0;
			}
		}
	}

	char *buffer = NULL;

	int length = 0;
	if ((length = ut_read_file(input, &buffer)) == -1) {
		ERROR(1, "Error reading input file");
	}
	INFO("Read %d bytes from %s\n", length, input);

	ut_dynamic_array_t tokens;
	ut_array_init(&tokens, sizeof(struct token));

	lexer_tokenize(buffer, length, &tokens);

	if (verbose_enabled) {
		for (unsigned int i = 0; i < tokens.len; i++) {
			struct token *t = ut_array_get(&tokens, i);
			print_token(t);
		}
	}

	INFO("Tokenized %d tokens\n", tokens.len);

	struct parser p;
	struct program_node program;

	ut_dynamic_array_t types_dict;
	ut_array_init(&types_dict, sizeof(struct type_dict));

	parser_init(tokens, types_dict, &p);

	parse_program(&p, &program);

	if (verbose_enabled) {
		print_instructions(&program.instructions, 0);
	}
	INFO("Parsed program with %d instructions\n", program.instructions.len);

	FILE *file = fopen(output, "w");
	if (file == NULL) {
		ERROR(1, "Error opening output file %s", output);
	}

	DEBUG("Output file opened: %s", output);

	program_asm(&program, file);

	fclose(file);
	DEBUG("Output file closed: %s", output);

	if (indent){
		INFO("Running formater on output file...\n");
		
		char *cmd = NULL;
		ut_str_cat(&cmd, "indent ", output, " -o ", output, " -linux -brs -brf -br", NULL);
		DEBUG("Running command: %s", cmd);
		int ret = system(cmd);
		if (ret != 0) {
			ERROR(1, "Error running formater on output file");
		}
		DEBUG("Formater finished successfully");	
	}


/*
 * Free Zone :)
 */

	for (unsigned int i = 0; i < program.instructions.len; i++) {
		struct instruction_node *instr =
		    ut_array_get(&program.instructions, i);
		if (instr->type == IF_STATEMENT
		    && instr->if_statement.body != NULL) {
			free(instr->if_statement.body);
			instr->if_statement.body = NULL;
		}
	}

	for (unsigned int i = 0; i < types_dict.len; i++) {
		struct type_dict *type = ut_array_get(&types_dict, i);
		free(type->name);
		type->name = NULL;
	}

	ut_array_free(&program.instructions);

	for (unsigned int i = 0; i < tokens.len; i++) {
		struct token *t = ut_array_get(&tokens, i);
		free(t->value);
		t->value = NULL;
	}
	ut_array_free(&tokens);

	cmdline_parser_free(&args);
	free(output);
	free(buffer);
	return 0;
}
