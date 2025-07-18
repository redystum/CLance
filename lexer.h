#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include "utils.h"

enum token_type {
	IDENT,			// identifier, like vars, types, etc
	INT,
	STRING,			// string inside ""
	UNPROCESSED_STRING,	// string inside ''
	INPUT,
	OUTPUT,
	IF,
	EQUAL,
	PLUS,
	LESS_THAN,
	GREATER_THAN,
	EQUALS_TO,
	DIRECTIVE,		// the '#' on includes
	TYPE,
	OPEN_BRACKET,
	CLOSE_BRACKET,
	OPEN_PAREN,
	CLOSE_PAREN,
	RETURN,
	INVALID,
	END,
};

typedef struct token {
	enum token_type type;
	char *value;
} token_t;

struct lexer {
	char *buffer;
	unsigned int buffer_len;
	unsigned int pos;
	unsigned int read_pos;
	char ch;
};

const char *show_token_type(enum token_type type);
void print_token(token_t * token);
char lexer_peek_char(struct lexer *l, int skip_whitespace);
char lexer_read_char(struct lexer *l);
void skip_whitespace(struct lexer *l);
void lexer_init(struct lexer *l, char *buffer, unsigned int buffer_len);
struct token lexer_next_token(struct lexer *l);
int lexer_tokenize(char *buffer, unsigned int len, ut_dynamic_array_t * tokens);
void lexer_free(ut_dynamic_array_t * tokens);

#endif
