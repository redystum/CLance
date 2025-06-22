#ifndef COMPILER_H
#define COMPILER_H

#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "string.h"


void program_asm(struct program_node *program, FILE *file);
void instr_asm(struct instruction_node *instr, FILE *file);
void asm_directive(struct instruction_node *instr, FILE *file);
void asm_print(struct instruction_node *instr, FILE *f);
void asm_type(struct instruction_node *instr, FILE *f);
void asm_assign(struct instruction_node *instr, FILE *f);
#endif // COMPILER_H