#ifndef PARSER_H
#define PARSER_H

#include "utils.h"
#include "lexer.h"

enum intruction_type {
    INSTRUCTION,
    VARIABLE,
    ASSIGN,
    RETURN_STATEMENT,
    IF_STATEMENT,
    PRINT_STATEMENT,
    INPUT_STATEMENT,
    END_STATEMENT,
    DIRECTIVE_STATEMENT,
    TYPE_STATEMENT,
};

enum term_type {
    INPUT_TERM,
    INT_TERM,
    IDENTIFIER_TERM,
    STRING_TERM,
};

enum types {
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

struct term_node {
    enum term_type type;
    union {
        char* value;
    };
};

struct term_binary_node {
    struct term_node left;
    struct term_node right;
};

struct input_node {
    char* prompt;
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
};

struct relation_node {
    enum relation_type type;
    union {
        struct term_binary_node greater_than;
    };
};

struct assign_node {
    char* identifier;
    struct expression_node expression;
};

struct if_node {
    struct relation_node rel;
    struct program_node* body;
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

struct instruction_node {
    enum intruction_type type;
    union {
        struct assign_node assign;
        struct if_node if_statement;
        struct print_node print_statement;
        struct return_node return_statement;
        struct input_node input_statement;
        struct type_node type_statement;
    };
};

struct program_node {
    ut_dynamic_array_t instructions;
};

struct type_dict {
    char* name;
    enum types type;
};

struct parser{
    ut_dynamic_array_t tokens;
    unsigned int index;
    ut_dynamic_array_t types_dict;
};

void parser_current(struct parser* p, struct token* token);
void parser_advance(struct parser* p);
void parse_instr(struct parser* p, struct instruction_node* instr);
void parse_term(struct parser* p, struct term_node* term);
void parse_exp(struct parser* p, struct expression_node* exp);
void parse_rel(struct parser* p, struct relation_node* rel);
void parse_assign(struct parser* p, struct instruction_node* instr);
void parse_if(struct parser* p, struct instruction_node* instr);
void parse_print(struct parser* p, struct instruction_node* instr);
void parse_input(struct parser* p, struct instruction_node* instr);
void parse_return(struct parser* p, struct instruction_node* instr);
void parse_program(struct parser* p, struct program_node* program);
void parser_init(ut_dynamic_array_t tokens, ut_dynamic_array_t types_dict, struct parser* p);
#endif // PARSER_H