#include "parser.h"
#include "lexer.h"
#include "utils.h"
#include "string.h"

const char *show_instruction_type(enum intruction_type type) {
	switch (type) {
	case INSTRUCTION:
		return "INSTRUCTION";
	case ASSIGN:
		return "ASSIGN";
	case RETURN_STATEMENT:
		return "RETURN";
	case IF_STATEMENT:
		return "IF";
	case PRINT_STATEMENT:
		return "PRINT";
	case INPUT_STATEMENT:
		return "INPUT";
	case END_STATEMENT:
		return "END";
	case DIRECTIVE_STATEMENT:
		return "DIRECTIVE";
	case TYPE_STATEMENT:
		return "TYPE";
	case EOL_STATEMENT:
		return "EOL";
	}
	return "UNKNOWN";
}

const char *show_types(enum types type) {
	switch (type) {
	case INT_TYPE:
		return "INT";
	case STRING_TYPE:
		return "STRING";
	case VOID_TYPE:
		return "VOID";
	case UNKNOWN_TYPE:
		return "UNKNOWN";
	case NULL_TYPE:
		return "NULL";
	}
	return "UNKNOWN";
}

void print_instructions(ut_dynamic_array_t *instructions, unsigned int deep) {

	for (unsigned int i = 0; i < instructions->len; i++) {
		struct instruction_node *inst = ut_array_get(instructions, i);
		printf("\n");
		print_w_deep(deep, "Instruction: %s\n",
			     show_instruction_type(inst->type));

		switch (inst->type) {
		case IF_STATEMENT:
			print_w_deep(deep,
				     " - If Statement with relation type: %d\n",
				     inst->if_statement.rel.type);
			if (inst->if_statement.rel.type ==
			    GREATER_THAN_RELATION) {
				print_w_deep(deep,
					     " - Greater Than Relation: %s > %s\n",
					     inst->if_statement.
					     rel.greater_than.left.value,
					     inst->if_statement.
					     rel.greater_than.right.value);
			}
			if (inst->if_statement.body != NULL
			    && inst->if_statement.body->instructions.len > 0) {
				print_w_deep(deep,
					     " - If Body Instructions:\n");
				print_instructions(&inst->if_statement.
						   body->instructions,
						   deep + 1);
			}

			break;
		case INSTRUCTION:
			print_w_deep(deep, " - Instruction\n");

			break;
		case ASSIGN:
			print_w_deep(deep, " - Assign: %s: %s\n",
				     inst->assign.identifier,
				     show_types(inst->type_statement.type));
			print_w_deep(deep, " - Expression Type: %d\n",
				     inst->assign.expression.type);
			switch (inst->assign.expression.type) {
			case PLUS_EXPRESSION:
				print_w_deep(deep,
					     " - Plus Expression: %s + %s\n",
					     inst->assign.expression.add.
					     left.value,
					     inst->assign.expression.add.
					     right.value);
				break;
			case INPUT_EXPRESSION:
				print_w_deep(deep,
					     " - Input Expression with prompt: %s\n",
					     inst->assign.expression.
					     input.prompt);
				break;
			case TERM_EXPRESSION:
				print_w_deep(deep, " - Term Expression: %s\n",
					     inst->assign.expression.
					     term.value);
				break;
			default:
				print_w_deep(deep,
					     " - Unknown Expression Type: %d\n",
					     inst->assign.expression.type);
			}
			break;
		case PRINT_STATEMENT:
			print_w_deep(deep, " - Print Statement: %s\n",
				     inst->print_statement.term.value);
			break;
		case INPUT_STATEMENT:
			print_w_deep(deep,
				     " - Input Statement with prompt: %s\n",
				     inst->input_statement.prompt);
			break;
		case RETURN_STATEMENT:
			print_w_deep(deep,
				     " - Return Statement with expression type: %d\n",
				     inst->return_statement.expression.type);
			break;
		case TYPE_STATEMENT:
			print_w_deep(deep, " - Type Statement: %d\n",
				     inst->type_statement.type);

			print_w_deep(deep + 1, " - Assign: %s: %s\n",
				     inst->assign.identifier,
				     show_types(inst->type_statement.type));
			print_w_deep(deep + 1, " - Expression Type: %d\n",
				     inst->assign.expression.type);
			switch (inst->assign.expression.type) {
			case PLUS_EXPRESSION:
				print_w_deep(deep + 1,
					     " - Plus Expression: %s + %s\n",
					     inst->assign.expression.add.
					     left.value,
					     inst->assign.expression.add.
					     right.value);
				break;
			case INPUT_EXPRESSION:
				print_w_deep(deep + 1,
					     " - Input Expression with prompt: %s\n",
					     inst->assign.expression.input.
					     prompt);
				break;
			case TERM_EXPRESSION:
				print_w_deep(deep + 1,
					     " - Term Expression: %s\n",
					     inst->assign.expression.
					     term.value);
				break;
			default:
				print_w_deep(deep + 1,
					     " - Unknown Expression Type: %d\n",
					     inst->assign.expression.type);
			}

			break;
		case EOL_STATEMENT:
			print_w_deep(deep, " - End of Line Statement\n");
			break;
		case DIRECTIVE_STATEMENT:
			print_w_deep(deep,
				     " - Directive Statement with identifier: %s\n",
				     inst->directive_statement.identifier);
			break;
		case END_STATEMENT:
			print_w_deep(deep, " - End of Program Statement\n");
			break;
		default:
			print_w_deep(deep, " - Unknown Instruction Type\n");
		}
	}
}

void parse_term(struct parser *p, struct term_node *term) {
	struct token token;

	parser_current(p, &token);
	if (token.type == INPUT) {
		term->type = INPUT_TERM;
		term->value = NULL;	// INPUT term does not have a value
	} else if (token.type == INT) {
		term->type = INT_TERM;
		term->value = token.value;
	} else if (token.type == IDENT) {
		term->type = IDENTIFIER_TERM;
		term->value = token.value;
	} else if (token.type == STRING) {
		term->type = STRING_TERM;
		term->value = token.value;
	} else {
		ERROR(1,
		      "term: Expected input, int, string or identifier, got %s",
		      show_token_type(token.type));
	}

	DEBUG("term: Parsed term with type %d and value '%s'", term->type,
	      term->value ? term->value : "NULL");
}

void parse_exp(struct parser *p, struct expression_node *exp) {
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

		parser_current(p, &token);
		if (token.type != OPEN_PAREN) {
			ERROR(1, "input: Expected '(', got %s",
			      show_token_type(token.type));
		}
		parser_advance(p);

		parser_current(p, &token);
		if (token.type != STRING) {
			ERROR(1, "input: Expected string prompt, got %s",
			      show_token_type(token.type));
		}

        exp->type = INPUT_EXPRESSION;
		exp->input.prompt = token.value;

		parser_advance(p);

		parser_current(p, &token);
		if (token.type != CLOSE_PAREN) {
			ERROR(1, "input: Expected ')', got %s",
			      show_token_type(token.type));
		}
	} else {
		exp->type = TERM_EXPRESSION;
		exp->term = left;
	}
	parser_advance(p);

	DEBUG("Parsed expression with type %d", exp->type);
}

void parse_assign(struct parser *p, struct instruction_node *instr) {
	struct token token;

	instr->type = ASSIGN;

	parser_current(p, &token);
	instr->assign.identifier = token.value;
	parser_advance(p);

	if (instr->type_statement.type != NULL_TYPE) {
        DEBUG("Type already set for identifier '%s', skipping type assignment",
              instr->assign.identifier);
		struct type_dict new_type = {
			.name = instr->assign.identifier,
			.type = instr->type_statement.type
		};
		ut_array_push(&p->types_dict, &new_type);
	} else {
        DEBUG("Type not set for identifier '%s', searching in types dictionary",
              instr->assign.identifier);
		int found = 0;
		for (unsigned int i = 0; i < p->types_dict.len; i++) {
			struct type_dict *type =
			    ut_array_get(&p->types_dict, i);
			if (strcmp(type->name, instr->assign.identifier) == 0) {
				instr->type_statement.type = type->type;
				found = 1;
				break;
			}
		}
		if (!found) {
			ERROR(1,
			      "assign: Identifier '%s' not found in types dictionary",
			      instr->assign.identifier);
		}
	}

	parser_current(p, &token);
	if (token.type != EQUAL) {
		ERROR(1, "assign: Expected '=', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	parse_exp(p, &instr->assign.expression);

	// todo: check if the expression type matches the identifier type

	DEBUG("Parsed assignment for identifier '%s', type %s : %d",
	      instr->assign.identifier, show_types(instr->type_statement.type), instr->type_statement.type);
}

void parse_rel(struct parser *p, struct relation_node *rel) {
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
		ERROR(1, "rel: Expected '>', got %s %i",
		      show_token_type(token.type), token.type);
	}

	parser_advance(p);
	DEBUG("Parsed relation with type %d", rel->type);
}

enum types enum_type(char *term) {
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

void parse_type(struct parser *p, struct instruction_node *instr) {
	struct token token;

	parser_current(p, &token);

	// instr->type_statement.type = enum_type(token.value);
	parser_advance(p);

	parse_assign(p, instr);

	instr->type = TYPE_STATEMENT;

	parser_current(p, &token);
	if (token.type != EOL_) {
		ERROR(1, "type: Expected end of line, got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	DEBUG("Parsed type: %s : %s", token.value, show_token_type(token.type));
}

void parse_if(struct parser *p, struct instruction_node *instr) {
	struct token token;

	instr->type = IF_STATEMENT;
	parser_advance(p);

	parser_current(p, &token);
	if (token.type != OPEN_PAREN) {
		ERROR(1, "if: Expected '(', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	parse_rel(p, &instr->if_statement.rel);

	parser_current(p, &token);
	if (token.type != CLOSE_PAREN) {
		ERROR(1, "if: Expected ')', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	parser_current(p, &token);
	if (token.type != OPEN_BRACKET) {
		ERROR(1, "if: Expected '{', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	struct program_node *if_body =
	    (struct program_node *)malloc(sizeof(struct program_node));
	if (!if_body) {
		ERROR(1, "if: Memory allocation failed for if body");
	}

	parse_program(p, if_body);
	instr->if_statement.body = if_body;

	parser_current(p, &token);
	DEBUG("Current token after body is %s, %i", show_token_type(token.type),
	      token.type);

	if (token.type != CLOSE_BRACKET) {
		ERROR(1, "if: Expected '}', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);
}

void parse_print(struct parser *p, struct instruction_node *instr) {
	struct token token;
	struct term_node term;

	instr->type = PRINT_STATEMENT;
	parser_advance(p);
	parser_current(p, &token);

	if (token.type != OPEN_PAREN) {
		ERROR(1, "print: Expected '(', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	parse_term(p, &term);
	instr->print_statement.term = term;

	parser_advance(p);
	parser_current(p, &token);
	if (token.type != CLOSE_PAREN) {
		ERROR(1, "print: Expected ')', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	DEBUG("Parsed print statement with term type %d and value '%s'",
	      term.type, term.value ? term.value : "NULL");
}

void parse_input(struct parser *p, struct instruction_node *instr) {
	struct token token;

	instr->type = INPUT_STATEMENT;
	parser_advance(p);

	parser_current(p, &token);
	if (token.type != OPEN_PAREN) {
		ERROR(1, "input: Expected '(', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	parser_current(p, &token);
	if (token.type != STRING) {
		ERROR(1, "input: Expected string, got %s",
		      show_token_type(token.type));
	}

	instr->input_statement.prompt = token.value;
	parser_advance(p);

	parser_current(p, &token);
	if (token.type != CLOSE_PAREN) {
		ERROR(1, "input: Expected ')', got %s",
		      show_token_type(token.type));
	}
	parser_advance(p);

	DEBUG("Parsed input statement for identifier '%s'",
	      instr->input_statement.prompt);
}

void parse_return(struct parser *p, struct instruction_node *instr) {
	struct expression_node exp;

	instr->type = RETURN_STATEMENT;
	parser_advance(p);
	parse_exp(p, &exp);
	instr->return_statement.expression = exp;

	parser_advance(p);

	DEBUG("Parsed return statement with expression type %d", exp.type);
}

void parse_directive(struct parser *p, struct instruction_node *instr) {
	struct token token;

	instr->type = DIRECTIVE_STATEMENT;

	parser_current(p, &token);
	instr->directive_statement.identifier = token.value;

	parser_advance(p);

	DEBUG("Parsed directive statement with identifier '%s'",
	      instr->directive_statement.identifier);
}

void parse_instr(struct parser *p, struct instruction_node *instr) {
	struct token token;
	parser_current(p, &token);

	DEBUG("Current token is %s %i", show_token_type(token.type),
	      token.type);

	if (token.type == IDENT) {
		parse_assign(p, instr);
	} else if (token.type == TYPE) {
		parse_type(p, instr);
	} else if (token.type == INPUT) {
		parse_input(p, instr);
	} else if (token.type == OUTPUT) {
		parse_print(p, instr);
	} else if (token.type == IF) {
		parse_if(p, instr);
	} else if (token.type == RETURN) {
		parse_return(p, instr);
	} else if (token.type == EOL_) {
		instr->type = EOL_STATEMENT;
		parser_advance(p);
	} else if (token.type == DIRECTIVE) {
		parse_directive(p, instr);
	} else if (token.type == CLOSE_BRACKET) {
		instr->type = END_STATEMENT;
	} else if (token.type == END) {
		instr->type = END_STATEMENT;
	} else if (token.type == INVALID) {
		ERROR(1, "Invalid token encountered: %s",
		      token.value ? token.value : "NULL");
	} else {
		ERROR(1, "Unexpected token: %s %i", show_token_type(token.type),
		      token.type);
	}

	DEBUG("Parsed instruction of type %d, token type %s\n", instr->type,
	      show_token_type(token.type));
}

void parse_program(struct parser *p, struct program_node *program) {
	ut_array_init(&program->instructions, sizeof(struct instruction_node));

	struct token token;
	do {
		struct instruction_node instr = { 0 };

		parse_instr(p, &instr);
		ut_array_push(&program->instructions, &instr);

		parser_current(p, &token);

		if (instr.type == END_STATEMENT) {
			break;
		}

	} while (token.type != END);
}

void parser_init(ut_dynamic_array_t tokens, ut_dynamic_array_t types_dict,
		 struct parser *p) {
	p->tokens = tokens;
	p->index = 0;
	p->types_dict = types_dict;
}

void parser_current(struct parser *p, struct token *token) {
	*token = *(struct token *)ut_array_get(&p->tokens, p->index);
}

void parser_advance(struct parser *p) {
	p->index++;
}
