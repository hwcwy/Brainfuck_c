#pragma once

#include <stdint.h>
#include "type.h"

/////////////////////// VEC  ///////////////////////

Vector* vector_new_unsafe(void);

void vector_free(Vector* vec);

void vector_push_unsafe(Vector* vec, uint32_t value);

uint32_t vector_pop_unsafe(Vector* vec);

//////////////////// TOKEN_VEC  ////////////////////

Token_vec* token_vec_new_unsafe();

void token_vec_free(Token_vec* vec);

void token_vec_push_unsafe(Token_vec* vec, Token value);

Token* token_vec_get_last(Token_vec* vec);