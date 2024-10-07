#include "interpreter.h"
#include "error.h"
#include "utils.h"
#include "token.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <wchar.h>

#include "lib.h"

const char* const REPL_HELP = "Commands:\n"
"exit         Exit REPL mode\n"
"clear        Clear memory\n"
"v            Enable verbose mode\n"
"uv           Disable verbose mode\n"
"? | help     Print this";

Exec_queue* Exec_queue_new_unsafe(Token_vec* token_vec) {
	Exec_queue* exec_queue = (Exec_queue*)malloc(sizeof(Exec_queue));
	if (exec_queue == NULL) {
		error_append_format("Failed to allocate memory for Exec_queue\n\tExec_queue_new_unsafe in interpreter.c");
		return;
	}

	exec_queue->view = token_vec;
	exec_queue->ptr = 0;

	return exec_queue;
}

void Exec_queue_jump_forward(Exec_queue* exec_queue, uint32_t n) {
	exec_queue->ptr = n;
}

void Exec_queue_jump_back(Exec_queue* exec_queue, uint32_t n) {
	exec_queue->ptr = n;
}

Token* Exec_queue_next_token(Exec_queue* exec_queue) {
	if ((size_t)exec_queue->ptr >= exec_queue->view->size) {
		return NULL;
	};

	Token* token_ptr = &exec_queue->view->data[exec_queue->ptr];
	exec_queue->ptr += 1;

	return token_ptr;
}

void interpreter_run(Config* config) {
	Memory* runtime_memory = Memory_new_unsafe(config->cell_max);
	bool with_buffer = config->output_mode == Bulk || config->verbose;
	IO* io = IO_new_unsafe(config->output_mode, with_buffer);

	if (error_check()) return error_append_format("\n\tinterpreter_run in interpreter.c");

	if (config->repl_mode) {
		printf("%s\n\n", REPL_HELP);
		Memory_display(runtime_memory);
		printf("\n\n");

		while (true) {
			repl_mode(&runtime_memory, &io, config->verbose);
			if (error_check()) {
				error_resolve();
				printf("Recovering from error: %s\n", global_error_msg);
				Memory_display(runtime_memory);
				printf("\n");

				wchar_t* str = IO_buffer_to_string_unsafe(io);
				if (error_check()) return error_append_format("\n\tinterpreter_run in interpreter.c");
				wprintf(L"%ls\n", str);
			}
			else break;
		}
	}
	else {
		Exec_queue* exec_queue = Exec_queue_new_unsafe(config->token_vec);
		normal_mode(runtime_memory, io, config->verbose, exec_queue);

		if (error_check()) return error_append_format("\n\tinterpreter_run in interpreter.c");
	}
}

void normal_mode(Memory* memory, IO* io, bool verbose, Exec_queue* exec_queue) {
	bool should_print_individually = !verbose && io->output_mode == Individually;
	while (true) {
		Token* token = Exec_queue_next_token(exec_queue);
		if (token == NULL) break;

		if (verbose) {
			Memory_display(memory);
			printf(" ");
		}

		switch (token->type)
		{
		case PTR_INCREASE: {
			Memory_ptr_increase_unsafe(memory, token->value);
			break;
		}
		case PTR_DECREASE: {
			Memory_ptr_decrease_unsafe(memory, token->value);
			break;
		}
		case DATA_INCREASE: {
			Memory_data_increase(memory, token->value);
			break;
		}
		case DATA_DECREASE: {
			Memory_data_decrease(memory, token->value);
			break;
		}
		case JUMP_FORWARD: {
			if (Memory_output(memory) == 0) {
				Exec_queue_jump_forward(exec_queue, token->value);
			}
			break;
		}
		case JUMP_BACK: {
			if (Memory_output(memory) != 0) {
				Exec_queue_jump_back(exec_queue, token->value);
			}
			break;
		}
		case OUTPUT: {
			wchar_t c = IO_output_unsafe(io, memory);
			if (should_print_individually) {
				wprintf(L"%lc", c);
				fflush(stdout);
			}
			break;
		}
		case INPUT: {
			IO_input_unsafe(io, memory);
			break;
		}
		}
		if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
		if (verbose) {
			if (token->type != INPUT) {
				println_token_type(token);
			}
			wchar_t* str = IO_buffer_to_string_unsafe(io);
			if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
			wprintf(L"%ls\n", str);
		}

	}

	if (!verbose && io->output_mode == Bulk) {
		wchar_t* str = IO_buffer_to_string_unsafe(io);
		if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
		wprintf(L"%ls", str);
	}
}

void repl_mode(Memory** memory, IO** io, bool verbose) {
	(*io)->output_buffer = vector_new_unsafe();
	if (error_check()) return error_append_format("\n\rrepl_mode in interpreter.c");

	while (true) {
		printf("> ");
		fflush(stdout);

		char buffer[repl_input_limit + 1];
		fgets(buffer, repl_input_limit, stdin);

		trim_end(buffer);

		if (buffer[0] == '\0') {
			printf("\n");
		}

		if (cmp(buffer, "exit")) break;
		else if (cmp(buffer, "clear")) {
			Memory* old_memory = *memory;
			*memory = Memory_new_unsafe((*memory)->cell_max);
			if (error_check()) return error_append_format("\n\rrepl_mode in interpreter.c");
			Memory_free(old_memory);
			IO* old_io = *io;
			*io = IO_new_unsafe((*io)->output_mode, true);
			if (error_check()) return error_append_format("\n\rrepl_mode in interpreter.c");
			IO_free(old_io);
		}
		else if (cmp(buffer, "v")) verbose = true;
		else if (cmp(buffer, "uv")) verbose = false;
		else if (cmp(buffer, "?") || cmp(buffer, "help")) printf("%s\n", REPL_HELP);

		Token_vec* token_vec = raw_code_to_token_vec_unsafe(buffer);
		if (error_check()) return error_append_format("\n\rrepl_mode in interpreter.c");
		Exec_queue* exec_queue = Exec_queue_new_unsafe(token_vec);
		if (error_check()) return error_append_format("\n\rrepl_mode in interpreter.c");

		bool should_print_individually = !verbose && (*io)->output_mode == Individually;
		while (true) {
			Token* token = Exec_queue_next_token(exec_queue);
			if (token == NULL) break;

			if (verbose) {
				Memory_display(memory);
				printf(" ");
			}

			switch (token->type)
			{
			case PTR_INCREASE: {
				Memory_ptr_increase_unsafe(*memory, token->value);
				break;
			}
			case PTR_DECREASE: {
				Memory_ptr_decrease_unsafe(*memory, token->value);
				break;
			}
			case DATA_INCREASE: {
				Memory_data_increase(*memory, token->value);
				break;
			}
			case DATA_DECREASE: {
				Memory_data_decrease(*memory, token->value);
				break;
			}
			case JUMP_FORWARD: {
				if (Memory_output(*memory) == 0) {
					Exec_queue_jump_forward(exec_queue, token->value);
				}
				break;
			}
			case JUMP_BACK: {
				if (Memory_output(*memory) != 0) {
					Exec_queue_jump_back(exec_queue, token->value);
				}
				break;
			}
			case OUTPUT: {
				wchar_t c = IO_output_unsafe(*io, *memory);
				if (should_print_individually) {
					wprintf(L"%lc", c);
					fflush(stdout);
				}
				break;
			}
			case INPUT: {
				IO_input_unsafe(*io, *memory);
				break;
			}
			}
			if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
			if (verbose) {
				if (token->type != INPUT) {
					println_token_type(token);
				}
				wchar_t* str = IO_buffer_to_string_unsafe(*io);
				if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
				wprintf(L"%ls\n", str);
			}

		}

		printf("\r");
		Memory_display(*memory);
		printf("\n");
		wchar_t* str = IO_buffer_to_string_unsafe(*io);
		if (error_check()) return error_append_format("\n\tnormal_mode in interpreter.c");
		wprintf(L"%ls\n", str);
	}
}


