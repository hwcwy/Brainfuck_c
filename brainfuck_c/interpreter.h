#pragma once

#include "type.h"

#define repl_input_limit (200)

extern const char* const REPL_HELP;

Exec_queue* Exec_queue_new_unsafe(Token_vec* token_vec);

void Exec_queue_jump_forward(Exec_queue* exec_queue, uint32_t n);

void Exec_queue_jump_back(Exec_queue* exec_queue, uint32_t n);

Token* Exec_queue_next_token(Exec_queue* exec_queue);

void interpreter_run(Config* config);

static void normal_mode(Memory* memory, IO* io, bool verbose, Exec_queue* exec_queue);

static void repl_mode(Memory** memory, IO** io, bool verbose);