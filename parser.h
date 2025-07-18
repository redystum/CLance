#ifndef PARSER_H
#define PARSER_H

#include "utils.h"
#include "lexer.h"
#include <string.h>

enum intruction_type {
	INSTRUCTION,
	ASSIGN,
	RETURN_STATEMENT,
	IF_STATEMENT,
	PRINT_STATEMENT,
	INPUT_STATEMENT,
	END_STATEMENT,
	DIRECTIVE_STATEMENT,
	TYPE_STATEMENT,
	EOL_STATEMENT,
};

enum term_type {
	INPUT_TERM,
	INT_TERM,
	IDENTIFIER_TERM,
	STRING_TERM,
	UNPROCESSED_STRING_TERM,
};

enum types {
	NULL_TYPE,
	INT_TYPE,
	STRING_TYPE,
	VOID_TYPE,
	UNKNOWN_TYPE,
};

enum expression_type {
	TERM_EXPRESSION,
	PLUS_EXPRESSION,
	INPUT_EXPRESSION,
};

struct term_input_node {
	struct input_node *input;
	enum types type;
};

struct term_node {
	enum term_type type;
	union {
		char *value;
		struct term_input_node input;
		ut_dynamic_array_t processed_string;
	};
};

struct term_binary_node {
	struct term_node left;
	struct term_node right;
};

struct input_node {
	char *prompt;
};

struct expression_node {
	enum expression_type type;
	union {
		struct term_binary_node add;
		struct term_node term;
		struct input_node input;
	};
};

enum relation_type {
	GREATER_THAN_RELATION,
	LESS_THAN_RELATION,
	EQUALS_TO_RELATION
};

struct relation_node {
	enum relation_type type;
	union {
		struct term_binary_node greater_than;
		struct term_binary_node less_than;
		struct term_binary_node equals_to;
	};
};

struct assign_node {
	char *identifier;
	struct expression_node expression;
};

struct if_node {
	struct relation_node rel;
	struct program_node *body;
};

struct print_node {
	struct term_node term;
};

struct return_node {
	struct expression_node expression;
};

struct type_node {
	enum types type;
};

struct directive_node {
	char *identifier;
};

struct instruction_node {
	enum intruction_type type;
	// union {
	struct assign_node assign;
	struct if_node if_statement;
	struct print_node print_statement;
	struct return_node return_statement;
	struct input_node input_statement;
	struct type_node type_statement;
	struct directive_node directive_statement;
	// };
};

struct instruction_list_element {
	enum intruction_type instruction;
	union {
		enum types type;
	};
};

struct program_node {
	ut_dynamic_array_t instructions;
};

struct type_dict {
	char *name;
	enum types type;
};

struct parser {
	ut_dynamic_array_t tokens;
	unsigned int index;
	ut_dynamic_array_t types_dict;
};

void parser_current(struct parser *p, struct token *token);
void parser_advance(struct parser *p);
void parse_instr(struct parser *p, struct instruction_node *instr);
void parse_term(struct parser *p, struct term_node *term);
void parse_exp(struct parser *p, struct expression_node *exp);
void parse_rel(struct parser *p, struct relation_node *rel);
void parse_assign(struct parser *p, struct instruction_node *instr);
void parse_if(struct parser *p, struct instruction_node *instr);
void parse_print(struct parser *p, struct instruction_node *instr);
void parse_input(struct parser *p, struct instruction_node *instr);
void parse_return(struct parser *p, struct instruction_node *instr);
void parse_program(struct parser *p, struct program_node *program);
void parse_directive(struct parser *p, struct instruction_node *instr);
void parse_type(struct parser *p, struct instruction_node *instr);
void parser_init(ut_dynamic_array_t tokens,
		 ut_dynamic_array_t types_dict, struct parser *p);
void print_instructions(ut_dynamic_array_t * instructions, unsigned int deep);
struct token *parser_peek(struct parser *p);
const char *show_instruction_type(enum intruction_type type);
enum types enum_type(char *term);
const char *show_types(enum types type);
#endif				// PARSER_H
