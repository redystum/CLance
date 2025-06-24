#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "string.h"

enum functions {
	INPUT_INT_FUNC,
	INPUT_STRING_FUNC,
};

struct state {
	char *func_file_c;
	char *func_file_h;
	ut_dynamic_array_t used_functions;
};

void program_asm(struct program_node *program, FILE * file, char *output);
void program_asm_loop(struct state *s, struct program_node *program,
		      FILE * file, int main);
void program_header(FILE * file, char *output);
void functions_files(struct state *s, char *output);
void instr_asm(struct state *s, struct instruction_node *instr, FILE * file);
void asm_directive(struct state *s, struct instruction_node *instr,
		   FILE * file);
void asm_print(struct state *s, struct instruction_node *instr, FILE * f);
void asm_type(struct state *s, struct instruction_node *instr, FILE * f);
void asm_assign(struct state *s, struct instruction_node *instr, FILE * f);
void asm_if(struct state *s, struct instruction_node *instr, FILE * f);
void asm_return(struct state *s, struct instruction_node *instr, FILE * f);
void close_functions(struct state *s);
#endif				// COMPILER_H
