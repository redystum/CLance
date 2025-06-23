#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "string.h"


void program_asm(struct program_node *program, FILE *file, ut_dynamic_array_t instructions_list);
void program_asm_loop(struct program_node *program, FILE *file, int main);
void program_header(FILE *file);
void instructions_functions(FILE *file, ut_dynamic_array_t instructions_list);
void instr_asm(struct instruction_node *instr, FILE *file);
void asm_directive(struct instruction_node *instr, FILE *file);
void asm_print(struct instruction_node *instr, FILE *f);
void asm_type(struct instruction_node *instr, FILE *f);
void asm_assign(struct instruction_node *instr, FILE *f);
void asm_if(struct instruction_node *instr, FILE *f);
void asm_return(struct instruction_node *instr, FILE *f);
#endif // COMPILER_H