#ifndef STRING_LEXER_H
#define STRING_LEXER_H

#include "lexer.h"

enum str_token_type {
	STR_TOKEN_TYPE_TEXT,
	STR_TOKEN_TYPE_IDENTIFIER,
	STR_TOKEN_TYPE_FUNCTION,
	STR_TOKEN_TYPE_END,
};

struct str_token {
	enum str_token_type type;
	union {
		char *value;
		ut_dynamic_array_t tokens;	// lexer tokens for functions
	};
};

struct str_lexer {
	char *buffer;
	unsigned int buffer_len;
	unsigned int pos;
	unsigned int read_pos;

	char ch;
};

void str_lexer_init(struct str_lexer *lexer, char *buffer,
		    unsigned int buffer_len);
struct str_token str_lexer_next_token(struct str_lexer *lexer);
void str_lexer_free(ut_dynamic_array_t * tokens);
const char *show_str_token_type(enum str_token_type type);
void print_str_token(struct str_token *token);
int str_lexer_tokenize(char *buffer, unsigned int len,
		       ut_dynamic_array_t * tokens);

#endif				// STRING_LEXER_H
