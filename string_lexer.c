#include <stdio.h>

#include "lexer.h"
#include "string_lexer.h"
#include "utils.h"
#include <ctype.h>

const char *show_str_token_type(enum str_token_type type) {
	switch (type) {
	case STR_TOKEN_TYPE_TEXT:
		return "Text";
	case STR_TOKEN_TYPE_IDENTIFIER:
		return "Identifier";
	case STR_TOKEN_TYPE_FUNCTION:
		return "Function";
	case STR_TOKEN_TYPE_END:
		return "End";
	default:
		return "Unknown";
	}
}
void print_str_token(struct str_token *token) {
	if (token->type == STR_TOKEN_TYPE_FUNCTION) {
		printf("String Token: %s, Tokens:\n",
		       show_str_token_type(token->type));
		for (unsigned int i = 0; i < token->tokens.len; i++) {
			struct token *t = ut_array_get(&token->tokens, i);
			printf("\t");
			print_token(t);
		}
		printf("\n");
		return;
	} else if (token->type == STR_TOKEN_TYPE_END) {
		printf("String Token: %s\n", show_str_token_type(token->type));
		return;
	}
	printf("String Token: %s (%s)\n", show_str_token_type(token->type),
	       token->value);
}

char str_lexer_peek_char(struct str_lexer *l) {
	if (l->read_pos >= l->buffer_len) {
		return EOF;
	}
	return l->buffer[l->read_pos];
}

char str_lexer_peek_prev_char(struct str_lexer *l) {
	if (l->pos <= 0) {
		return EOF;
	}
	return l->buffer[l->pos - 1];
}

char str_lexer_read_char(struct str_lexer *l) {
	l->ch = str_lexer_peek_char(l);

	l->pos = l->read_pos;
	l->read_pos++;

	return l->ch;
}

void str_lexer_init(struct str_lexer *l, char *buffer, unsigned int buffer_len) {
	l->buffer = buffer;
	l->buffer_len = buffer_len;
	l->pos = 0;
	l->read_pos = 0;
	l->ch = 0;

	str_lexer_read_char(l);
}

struct str_token str_lexer_next_token(struct str_lexer *l) {

	if (l->ch == '$' && str_lexer_peek_prev_char(l) != '\\') {
		DEBUG("Found '$' at position %d", l->pos);
		str_lexer_read_char(l);

		if (l->ch == '{') {
			str_lexer_read_char(l);	// consume '{'

			if (l->ch == EOF) {
				ERROR(1, "Unmatched '$' in string\n");
			}

			if (l->ch == '}') {
				ERROR(1, "Empty identifier in string\n");
			}

			ut_string_slice_t slice = {.str =
				    l->buffer + l->pos,.len = 0
			};

			DEBUG("Reading function string at position %d", l->pos);

			while (l->ch != '}') {
				if (l->ch == EOF) {
					ERROR(1, "Unmatched '$' in string\n");
				} else if (l->pos >= l->buffer_len) {
					ERROR(1, "Unmatched '$' in string\n");
				}

				slice.len += 1;
				str_lexer_read_char(l);
			}

			if (l->ch == '}') {
				str_lexer_read_char(l);	// consume '}'
			} else {
				ERROR(1, "Unmatched '$' in string\n");
			}

			char *val = NULL;
			ut_string_slice_original(&slice, &val);

			if (val == NULL) {
				ERROR(1, "String Function is NULL\n");
			}

			DEBUG("Function string read: %.*s", slice.len,
			      slice.str);
			ut_dynamic_array_t tokens;
			ut_array_init(&tokens, sizeof(struct token));

			if (lexer_tokenize(val, slice.len, &tokens) != 0) {
				ERROR(1,
				      "Failed to tokenize function string\n");
			}

			if (tokens.len == 0) {
				ERROR(1, "Function string is empty\n");
			}

			DEBUG("Function string tokenized with %d tokens",
			      tokens.len);

			return (struct str_token) {
				.type = STR_TOKEN_TYPE_FUNCTION,
				.tokens = tokens
			};
		}

		ut_string_slice_t slice = {.str = l->buffer + l->pos,.len = 0 };
		while (isalnum(l->ch) && l->ch != ' ' && l->ch != '$'
		       && l->ch != EOF && l->pos < l->buffer_len) {
			slice.len += 1;
			str_lexer_read_char(l);
		}

		char *val = NULL;
		ut_string_slice_original(&slice, &val);
		return (struct str_token) {.type =
			    STR_TOKEN_TYPE_IDENTIFIER,.value = val
		};

	} else if (l->ch == EOF) {
		DEBUG("Found EOF at position %d", l->pos);
		str_lexer_read_char(l);
		return (struct str_token) {.type = STR_TOKEN_TYPE_END,.value =
			    NULL
		};
	}

	DEBUG("Found text at position %d", l->pos);

	ut_string_slice_t slice = {.str = l->buffer + l->pos,.len = 0 };

	while (l->ch != '$' && l->ch != EOF && l->pos < l->buffer_len) {
		slice.len += 1;
		str_lexer_read_char(l);
	}

	char *val = NULL;
	ut_string_slice_original(&slice, &val);

	//? str_lexer_read_char(l);

	return (struct str_token) {.type = STR_TOKEN_TYPE_TEXT,.value = val };
}

int str_lexer_tokenize(char *buffer, unsigned int len,
		       ut_dynamic_array_t *tokens) {
	struct str_lexer lexer;
	str_lexer_init(&lexer, buffer, len);

	struct str_token tok = { 0 };
	do {
		tok = str_lexer_next_token(&lexer);
		ut_array_push(tokens, &tok);
	} while (tok.type != STR_TOKEN_TYPE_END);

	return 0;
}

void str_lexer_free(ut_dynamic_array_t *tokens) {
	for (unsigned int i = 0; i < tokens->len; i++) {
		struct str_token *t = ut_array_get(tokens, i);
		if (t->type == STR_TOKEN_TYPE_FUNCTION) {
			lexer_free(&t->tokens);
		}
		free(t->value);
		t->value = NULL;
	}
	ut_array_free(tokens);
}
