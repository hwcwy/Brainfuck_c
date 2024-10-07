#include "vector.h"
#include "error.h"

#include <stdlib.h>
#include <stdio.h>

/////////////////////// VEC  ///////////////////////

Vector* vector_new_unsafe(void) {
	Vector* vec = malloc(sizeof(Vector));
	if (vec == NULL) {
		error_append_format("Failed to allocate memory for vector\n\tvector_new_unsafe in vector.c");
		return;
	}

	vec->size = 0;
	vec->capacity = 4;

	vec->data = (uint32_t*)calloc(vec->capacity, sizeof(uint32_t));
	if (vec->data == NULL) {
		free(vec);
		error_append_format("Failed to initialize vector data\n\tvector_new_unsafe in vector.c");
		return;
	}

	return vec;
}

void vector_free(Vector* vec) {
	if (vec != NULL) {
		free(vec->data);
		free(vec);
	}
}

void vector_push_unsafe(Vector* vec, uint32_t value) {
	if (vec->size == vec->capacity) {
		void* temp_ptr = realloc(vec->data, vec->capacity * 2 * sizeof(uint32_t));
		if (temp_ptr == NULL) {
			error_append_format("Failed to realloc memory when extending vector\n\tvector_push_unsafe in vector.c");
			return;
		}
		vec->capacity *= 2;
		vec->data = (uint32_t*)temp_ptr;
	}
	vec->data[vec->size++] = value;
}

uint32_t vector_pop_unsafe(Vector* vec) {
	if (vec->size == 0) {
		error_append_format("Error: Attempt to pop from an empty vector\n\tvector_pop_unsafe in vector.c");
		return;
	}
	vec->size -= 1;
	return vec->data[vec->size];
}

//////////////////// TOKEN_VEC  ////////////////////

Token_vec* token_vec_new_unsafe() {
	Token_vec* vec = malloc(sizeof(Token_vec));
	if (vec == NULL) {
		error_append_format("Failed to allocate memory for token_vec\n\token_vec_new_unsafe in vector.c");
		return;
	}

	vec->size = 0;
	vec->capacity = 4;

	vec->data = (Token*)malloc(sizeof(Token) * vec->capacity);
	if (vec->data == NULL) {
		free(vec);
		error_append_format("Failed to initialize token_vec data\n\token_vec_new_unsafe in vector.c");
		return;
	}

	return vec;
}

void token_vec_free(Token_vec* vec) {
	if (vec != NULL) {
		free(vec->data);
		free(vec);
	}
}

void token_vec_push_unsafe(Token_vec* vec, Token value) {
	if (vec->size == vec->capacity) {
		void* temp_ptr = realloc(vec->data, vec->capacity * 2 * sizeof(Token));
		if (temp_ptr == NULL) {
			error_append_format("Failed to realloc memory when extending token_vec\n\token_vec_push_unsafe in vector.c");
			return;
		}
		vec->capacity *= 2;
		vec->data = (Token*)temp_ptr;
	}
	vec->data[vec->size++] = value;
}

Token* token_vec_get_last(Token_vec* vec) {
	if (vec->size == 0) {
		return NULL;
	}
	return &vec->data[vec->size - 1];
}