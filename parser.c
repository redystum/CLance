#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "string.h"

void parse_term(struct parser* p, struct term_node* term)
{
    struct token token;

    parser_current(p, &token);
    if (token.type == INPUT) {
        term->type = INPUT_TERM;
        term->value = NULL; // INPUT term does not have a value
    } else if (token.type == INT) {
        term->type = INT_TERM;
        term->value = token.value;
    } else if (token.type == IDENT) {
        term->type = IDENTIFIER_TERM;
        term->value = token.value;
    }else if (token.type == STRING) {
        term->type = STRING_TERM;
        term->value = token.value;
    } else {
        ERROR(1, "term: Expected input, int, string or identifier, got %s", show_token_type(token.type));
    }

    DEBUG("term: Parsed term with type %d and value '%s'", term->type, term->value ? term->value : "NULL");
}

void parse_exp(struct parser* p, struct expression_node* exp)
{
    struct token token;
    struct term_node left, right;

    parse_term(p, &left);

    parser_current(p, &token);
    if (token.type == PLUS) {
        parser_advance(p);
        parse_term(p, &right);

        exp->add.left = left;
        exp->add.right = right;
        exp->type = PLUS_EXPRESSION;
    } else if (token.type == INPUT) {
        parser_advance(p);
        
        // Check for opening parenthesis
        parser_current(p, &token);
        if (token.type != OPEN_PAREN) {
            ERROR(1, "input: Expected '(', got %s", show_token_type(token.type));
        }
        parser_advance(p);
        
        // Get the prompt string
        parser_current(p, &token);
        if (token.type != STRING) {
            ERROR(1, "input: Expected string prompt, got %s", show_token_type(token.type));
        }
        
        // Setup input expression
        exp->type = INPUT_EXPRESSION;
        exp->input.prompt = token.value;
        
        parser_advance(p);
        
        // Check for closing parenthesis
        parser_current(p, &token);
        if (token.type != CLOSE_PAREN) {
            ERROR(1, "input: Expected ')', got %s", show_token_type(token.type));
        }
    }
    else {
        exp->type = TERM_EXPRESSION;
        exp->term = left;
    }
    parser_advance(p);

    DEBUG("Parsed expression with type %d", exp->type);
}

void parse_assign(struct parser* p, struct instruction_node* instr)
{
    struct token token;

    instr->type = ASSIGN;

    parser_current(p, &token);
    instr->assign.identifier = token.value;
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != EQUAL) {
        ERROR(1, "assign: Expected '=', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    parse_exp(p, &instr->assign.expression);

    DEBUG("Parsed assignment for identifier '%s'", instr->assign.identifier);
}

void parse_rel(struct parser* p, struct relation_node* rel)
{
    struct token token;
    struct term_node left, right;

    parse_term(p, &left);
    parser_advance(p);

    parser_current(p, &token);
    if (token.type == GREATER_THAN) {
        parser_advance(p);
        parse_term(p, &right);

        rel->type = GREATER_THAN_RELATION;
        rel->greater_than.left = left;
        rel->greater_than.right = right;
    } else {
       ERROR(1, "rel: Expected '>', got %s %i", show_token_type(token.type), token.type);
    }

    parser_advance(p);
    DEBUG("Parsed relation with type %d", rel->type);
}

enum types enum_type(char* term)
{
    if (strcmp(term, "int") == 0) {
        return INT_TYPE;
    } else if (strcmp(term, "string") == 0) {
        return STRING_TYPE;
    } else if (strcmp(term, "void") == 0) {
        return VOID_TYPE;
    } else {
        INFO("Unknown type: %s", term);
        return UNKNOWN_TYPE;
    }
}

void parse_type(struct parser* p, struct instruction_node* instr, struct token original_token)
{
    struct token token;
    struct term_node term;

    instr->type = TYPE_STATEMENT;
    instr->type_statement.type = enum_type(original_token.value);
    DEBUG("Parsed type: %s : %s", original_token.value, show_token_type(original_token.type));
    parser_advance(p);

    parse_assign(p, instr);

    parser_current(p, &token);
    if (token.type != EOL_) {
        ERROR(1, "type: Expected end of line, got %s", show_token_type(token.type));
    }
    parser_advance(p);    
}

void parse_if(struct parser* p, struct instruction_node* instr)
{
    struct token token;

    instr->type = IF_STATEMENT;
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != OPEN_PAREN) {
        ERROR(1, "if: Expected '(', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    parse_rel(p, &instr->if_statement.rel);

    parser_current(p, &token);
    if (token.type != CLOSE_PAREN) {
        ERROR(1, "if: Expected ')', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != OPEN_BRACKET) {
        ERROR(1, "if: Expected '{', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    struct program_node if_body;
    parse_program(p, &if_body);
    
    // parse_instr(p, instr->if_statement.body);
    instr->if_statement.body = &if_body;

    parser_current(p, &token);
    DEBUG("Current token after body is %s, %i", show_token_type(token.type), token.type);

    if (token.type != CLOSE_BRACKET) {
        ERROR(1, "if: Expected '}', got %s", show_token_type(token.type));
    }
    parser_advance(p);
}

void parse_print(struct parser* p, struct instruction_node* instr)
{
    struct token token;
    struct term_node term;

    instr->type = PRINT_STATEMENT;
    parser_advance(p);
    parser_current(p, &token);

    if (token.type != OPEN_PAREN) {
        ERROR(1, "print: Expected '(', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    parse_term(p, &term);
    instr->print_statement.term = term;

    parser_advance(p);
    parser_current(p, &token);
    if (token.type != CLOSE_PAREN) {
        ERROR(1, "print: Expected ')', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    DEBUG("Parsed print statement with term type %d and value '%s'", term.type, term.value ? term.value : "NULL");
}

void parse_input(struct parser* p, struct instruction_node* instr)
{
    struct token token;

    instr->type = INPUT_STATEMENT;
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != OPEN_PAREN) {
        ERROR(1, "input: Expected '(', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != STRING) {
        ERROR(1, "input: Expected string, got %s", show_token_type(token.type));
    }
    
    instr->input_statement.prompt = token.value;
    parser_advance(p);

    parser_current(p, &token);
    if (token.type != CLOSE_PAREN) {
        ERROR(1, "input: Expected ')', got %s", show_token_type(token.type));
    }
    parser_advance(p);

    DEBUG("Parsed input statement for identifier '%s'", instr->input_statement.prompt);
}

void parse_return(struct parser* p, struct instruction_node* instr)
{
    struct expression_node exp;

    instr->type = RETURN_STATEMENT;
    parser_advance(p);
    parse_exp(p, &exp);
    instr->return_statement.expression = exp;

    parser_advance(p);

    DEBUG("Parsed return statement with expression type %d", exp.type);
}

void parse_instr(struct parser* p, struct instruction_node* instr)
{
    struct token token;
    parser_current(p, &token);

    DEBUG("Current token is %s %i", show_token_type(token.type), token.type);

    if (token.type == IDENT) {
        parse_assign(p, instr);
    } else if (token.type == TYPE) {
        parse_type(p, instr, token);
    } else if (token.type == INPUT) {
        parse_input(p, instr);
    } else if (token.type == OUTPUT) {
        parse_print(p, instr);
    } else if (token.type == IF) {
        parse_if(p, instr);
    } else if (token.type == RETURN) {
        parse_return(p, instr);
    } else if (token.type == EOL_) {
        parser_advance(p);
    } else if (token.type == DIRECTIVE) {
        // todo
        instr->type = DIRECTIVE_STATEMENT;
        parser_advance(p);
    } else if (token.type == CLOSE_BRACKET) {
        instr->type = END_STATEMENT;
    } else if (token.type == END) {
        instr->type = END_STATEMENT;
    } else if (token.type == INVALID) {
        ERROR(1, "instr: Invalid token encountered: %s", token.value ? token.value : "NULL");
    } else {
        ERROR(1, "instr: Unexpected token: %s %i", show_token_type(token.type), token.type);
    }

    DEBUG("Parsed instruction of type %d, token type %s\n", instr->type, show_token_type(token.type));
}

void parse_program(struct parser* p, struct program_node* program)
{
    ut_array_init(&program->instructions, sizeof(struct instruction_node));

    struct token token;
    do {

        struct instruction_node instr;

        parse_instr(p, &instr);
        ut_array_push(&program->instructions, &instr);

        parser_current(p, &token);

        if (instr.type == END_STATEMENT) {
            break;
        }

    } while (token.type != END);
}

void parser_init(ut_dynamic_array_t tokens, ut_dynamic_array_t types_dict, struct parser* p)
{
    p->tokens = tokens;
    p->index = 0;
    p->types_dict = types_dict;
}

void parser_current(struct parser* p, struct token* token)
{
    *token = *(struct token*)ut_array_get(&p->tokens, p->index);
}

void parser_advance(struct parser* p)
{
    p->index++;
}