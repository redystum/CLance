#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

const char* show_token_type(enum token_type type)
{
    switch (type) {
    case IDENT:
        return "IDENTIFIER";
    case INT:
        return "INT";
    case STRING:
        return "STRING";
    case INPUT:
        return "INPUT";
    case OUTPUT:
        return "OUTPUT";
    case IF:
        return "IF";
    case EQUAL:
        return "EQUAL";
    case PLUS:
        return "PLUS";
    case LESS_THAN:
        return "LESS_THAN";
    case GREATER_THAN:
        return "GREATER_THAN";
    case DIRECTIVE:
        return "DIRECTIVE";
    case TYPE:
        return "TYPE";
    case OPEN_BRACKET:
        return "OPEN_BRACKET";
    case CLOSE_BRACKET:
        return "CLOSE_BRACKET";
    case OPEN_PAREN:
        return "OPEN_PAREN";
    case CLOSE_PAREN:
        return "CLOSE_PAREN";
    case RETURN:
        return "RETURN";
    case INVALID:
        return "INVALID";
    case END:
        return "END";
    }
    return "UNKNOWN";
}

void print_token(token_t* token)
{
    printf("Token: %s", show_token_type(token->type));
    if (token->value != NULL) {
        printf(" (%s)\n", token->value);
    } else {
        printf("\n");
    }
}

char lexer_peek_char(struct lexer* l, int skip_whitespace)
{
    if (skip_whitespace == 1) {
        while (isspace(l->buffer[l->read_pos])) {
            l->read_pos++;
        }
    }
    if (l->read_pos >= l->buffer_len) {
        return EOF;
    }
    return l->buffer[l->read_pos];
}

char lexer_read_char(struct lexer* l)
{
    l->ch = lexer_peek_char(l, 0);

    l->pos = l->read_pos;
    l->read_pos++;

    return l->ch;
}

void skip_whitespace(struct lexer* l)
{
    while (isspace(l->ch)) {
        lexer_read_char(l);
    }
}

void lexer_init(struct lexer* l, char* buffer, unsigned int buffer_len)
{
    l->buffer = buffer;
    l->buffer_len = buffer_len;
    l->pos = 0;
    l->read_pos = 0;
    l->ch = 0;

    lexer_read_char(l);
}

struct token lexer_next_token(struct lexer* l)
{
    skip_whitespace(l);

    if (l->ch == EOF) {
        lexer_read_char(l);
        return (struct token) { .type = END, .value = NULL };
    } else if (l->ch == '=') {
        lexer_read_char(l);
        return (struct token) { .type = EQUAL, .value = NULL };
    } else if (l->ch == '+') {
        lexer_read_char(l);
        return (struct token) { .type = EQUAL, .value = NULL };
    } else if (l->ch == '<') {
        lexer_read_char(l);
        return (struct token) { .type = LESS_THAN, .value = NULL };
    } else if (l->ch == '>') {
        lexer_read_char(l);
        return (struct token) { .type = GREATER_THAN, .value = NULL };
    } else if (l->ch == '"') {
        lexer_read_char(l);
        ut_string_slice_t slice = { .str = l->buffer + l->pos, .len = 0 };
        while (l->ch != EOF && l->ch != '"') {
            slice.len += 1;
            lexer_read_char(l);
        }
        char* val = NULL;
        ut_string_slice_original(&slice, &val);
        lexer_read_char(l);
        return (struct token) { .type = STRING, .value = val };
    } else if (l->ch == '#') {
        lexer_read_char(l);
        ut_string_slice_t slice = { .str = l->buffer + l->pos, .len = 0 };
        while (l->ch != EOF && l->ch != EOL) {
            slice.len += 1;
            lexer_read_char(l);
        }
        char* val = NULL;
        ut_string_slice_original(&slice, &val);
        return (struct token) { .type = DIRECTIVE, .value = val };
    } else if (isdigit(l->ch)) {
        ut_string_slice_t slice = { .str = l->buffer + l->pos, .len = 0 };
        while (l->ch != EOF && isdigit(l->ch)) {
            slice.len += 1;
            lexer_read_char(l);
        }
        char* val = NULL;
        ut_string_slice_original(&slice, &val);
        return (struct token) { .type = INT, .value = val };
    } else if (l->ch == '{') {
        lexer_read_char(l);
        return (struct token) { .type = OPEN_BRACKET, .value = NULL };
    } else if (l->ch == '}') {
        lexer_read_char(l);
        return (struct token) { .type = CLOSE_BRACKET, .value = NULL };
    } else if (l->ch == '(') {
        lexer_read_char(l);
        return (struct token) { .type = OPEN_PAREN, .value = NULL };
    } else if (l->ch == ')') {
        lexer_read_char(l);
        return (struct token) { .type = CLOSE_PAREN, .value = NULL };
    } else if (isalnum(l->ch) || l->ch == '_') {
        ut_string_slice_t slice = { .str = l->buffer + l->pos, .len = 0 };
        while (l->ch != EOF && l->ch != EOL && (isalnum(l->ch) || l->ch == '_')) {
            slice.len += 1;
            lexer_read_char(l);
        }
        char* val = NULL;
        ut_string_slice_original(&slice, &val);

        if (strcmp(val, "input") == 0) {
            return (struct token) { .type = INPUT, .value = val };
        } else if (strcmp(val, "print") == 0) {
            return (struct token) { .type = OUTPUT, .value = val };
        } else if (strcmp(val, "if") == 0) {
            return (struct token) { .type = IF, .value = val };
        } else if (strcmp(val, "return") == 0) {
            return (struct token) { .type = RETURN, .value = NULL };
        } else {
            char next_char = lexer_peek_char(l, 1);
            if (next_char != EOL && next_char != EOF && (isalnum(next_char) || next_char == '_')) {
                return (struct token) { .type = TYPE, .value = val };
            }

            return (struct token) { .type = IDENT, .value = val };
        }

    } else {
        ut_string_slice_t slice = { .str = l->buffer + l->pos, .len = 1 };
        char* value = NULL;
        ut_string_slice_original(&slice, &value);
        lexer_read_char(l);
        return (struct token) { .type = INVALID, .value = value };
    }
}

int lexer_tokenize(char* buffer, unsigned int len, ut_dynamic_array_t* tokens)
{

    struct lexer lexer;
    lexer_init(&lexer, (char*)buffer, len);

    struct token tok;
    do {
        tok = lexer_next_token(&lexer);
        ut_array_push(tokens, &tok);
    } while (tok.type != END);

    return 0;
}