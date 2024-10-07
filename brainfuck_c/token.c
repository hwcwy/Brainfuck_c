#include "token.h"
#include "error.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


Token_vec* raw_code_to_token_vec_unsafe(char* raw_code) {
	Token_vec* vec = token_vec_new_unsafe();
	if (error_check()) {
		error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
		return;
	}
	Vector* stack = vector_new_unsafe();
	if (error_check()) {
		error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
		return;
	}

	uint32_t line = 1;
	uint32_t col = 0;

	size_t len = strlen(raw_code);

	for (uint32_t i = 0; i < len; i++) {
		char c = raw_code[i];
		col += 1;

		switch (c) {
		case '>': {
			Token* token = token_vec_get_last(vec);
			if (token == NULL || token->type != PTR_INCREASE) {
				token_vec_push_unsafe(vec, (Token) { .type = PTR_INCREASE, .value = 1 });
				if (error_check()) {
					error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
					return;
				}
			}
			else {
				token->value += 1;
			}
			break;
		}

		case '<': {
			Token* token = token_vec_get_last(vec);
			if (token == NULL || token->type != PTR_DECREASE) {
				token_vec_push_unsafe(vec, (Token) { .type = PTR_DECREASE, .value = 1 });
				if (error_check()) {
					error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
					return;
				}
			}
			else {
				token->value += 1;
			}
			break;
		}

		case '+': {
			Token* token = token_vec_get_last(vec);
			if (token == NULL || token->type != DATA_INCREASE) {
				token_vec_push_unsafe(vec, (Token) { .type = DATA_INCREASE, .value = 1 });
				if (error_check()) {
					error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
					return;
				}
			}
			else {
				token->value += 1;
			}
			break;
		}

		case '-': {
			Token* token = token_vec_get_last(vec);
			if (token == NULL || token->type != DATA_DECREASE) {
				token_vec_push_unsafe(vec, (Token) { .type = DATA_DECREASE, .value = 1 });
				if (error_check()) {
					error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
					return;
				}
			}
			else {
				token->value += 1;
			}
			break;
		}

		case '.': {
			token_vec_push_unsafe(vec, (Token) { .type = OUTPUT, .value = 0 });
			if (error_check()) {
				error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
				return;
			}
			break;
		}

		case ',': {
			token_vec_push_unsafe(vec, (Token) { .type = INPUT, .value = 0 });
			if (error_check()) {
				error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
				return;
			}
			break;
		}

		case '[': {
			token_vec_push_unsafe(vec, (Token) { .type = JUMP_FORWARD, .value = 0 });
			if (error_check()) {
				error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
				return;
			}
			vector_push_unsafe(stack, vec->size);
			if (error_check()) {
				error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
				return;
			}
			break;
		}

		case ']': {
			uint32_t start = vector_pop_unsafe(stack);
			if (error_check()) {
				error_append_format("\n\tCompile error: UnexpectedRightBracket at line %" PRIu32":%" PRIu32 "\n\traw_code_to_token_vec_unsafe in token.c", line, col);
				return;
			}
			else {
				token_vec_push_unsafe(vec, (Token) { .type = JUMP_BACK, .value = start });
				if (error_check()) {
					error_append_format("\n\traw_code_to_token_vec_unsafe in token.c");
					return;
				}
				vec->data[start - 1].value = vec->size;
			}
			break;
		}
		case '\n': {
			line += 1;
			col = 0;
			break;
		}
		}
	}
	if (stack->size != 0) {
		error_append_format("Compile error: UnclosedLeftBracket at line %" PRIu32":%" PRIu32 "\n\traw_code_to_token_vec_unsafe in token.c", line, col);
		return;
	}

	return vec;
}

void println_token_type(Token* token) {
	switch (token->type) {
	case PTR_INCREASE: printf("PTR_INCREASE(%" PRIu32 ")\n", token->value); break;
	case PTR_DECREASE:  printf("PTR_DECREASE(%" PRIu32 ")\n", token->value); break;
	case DATA_INCREASE:  printf("DATA_INCREASE(%" PRIu32 ")\n", token->value); break;
	case DATA_DECREASE: printf("DATA_DECREASE(%" PRIu32 ")\n", token->value); break;
	case JUMP_FORWARD: printf("JUMP_FORWARD(%" PRIu32 ")\n", token->value); break;
	case JUMP_BACK: printf("JUMP_BACK(%" PRIu32 ")\n", token->value); break;
	case OUTPUT: printf("OUTPUT\n"); break;
	case INPUT: printf("INPUT\n"); break;
	}
}

