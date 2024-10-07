#include "lib.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

//////////////////// MEMORY  ////////////////////

Memory* Memory_new_unsafe(uint32_t cell_max) {
	Memory* memory = (Memory*)malloc(sizeof(Memory));
	if (memory == NULL) {
		error_append_format("Failed to allocate memory for Memory\n\tMemory_new_unsafe in lib.c");
		return;
	}
	memory->cell_max = cell_max;
	memory->ptr = 0;
	memory->view = vector_new_unsafe();
	if (error_check()) {
		free(memory);
		error_append_format("\n\tMemory_new_unsafe in lib.c");
		return;
	}
	vector_push_unsafe(memory->view, 0);
	if (error_check()) {
		free(memory);
		error_append_format("\n\tMemory_new_unsafe in lib.c");
		return;
	}
	return memory;
}

void Memory_display(Memory* memory) {
	printf("[");
	for (size_t i = 0; i < memory->view->size; i++) {
		if (i == memory->ptr) {
			printf(">");
		}
		printf("%" PRIu32, memory->view->data[i]);
		if (i != memory->view->size - 1) {
			printf(", ");
		}
	}
	printf("]");
}

void Memory_ptr_increase_unsafe(Memory* memory, uint32_t n) {
	while (n--) {
		memory->ptr += 1;
		if (memory->ptr == memory->view->size) {
			vector_push_unsafe(memory->view, 0);
			if (error_check()) return error_append_format("\n\tMemory_ptr_increase_unsafe in lib.c");
		};
	}
}

void Memory_ptr_decrease_unsafe(Memory* memory, uint32_t n) {
	if (n > memory->ptr) {
		error_append_format("The current pointer is at position %" PRIu32 " and cannot move left by %" PRIu32 " positions\n\tMemory_ptr_decrease_unsafe in lib.c", memory->ptr, n);
		return;
	}
	memory->ptr -= n;
}

void Memory_data_increase(Memory* memory, uint32_t n) {
	uint32_t* v = &memory->view->data[memory->ptr];
	if (memory->cell_max - *v >= n) {
		*v = *v + n;
	}
	else {
		*v = n - (memory->cell_max - *v + 1);
	}
}

void Memory_data_decrease(Memory* memory, uint32_t n) {
	uint32_t* v = &memory->view->data[memory->ptr];
	if (*v >= n) {
		*v = *v - n;
	}
	else {
		*v = memory->cell_max - (n - *v - 1);
	}
}

uint32_t Memory_output(Memory* memory) {
	return memory->view->data[memory->ptr];
}

void Memory_input(Memory* memory, uint32_t n) {
	memory->view->data[memory->ptr] = n;
}

void Memory_free(Memory* memory) {
	if (memory != NULL) {
		vector_free(memory->view);
		free(memory);
	}
}

////////////////////// IO  //////////////////////

IO* IO_new_unsafe(Output_mode output_mode, bool with_buffer) {
	IO* io = (IO*)malloc(sizeof(IO));
	if (io == NULL) {
		error_append_format("Failed to allocate memory for IO\n\tIO_new_unsafe in lib.c");
		return;
	}
	io->output_mode = output_mode;
	io->output_buffer = with_buffer ? vector_new_unsafe() : NULL;

	if (error_check()) {
		free(io);
		error_append_format("\n\tIO_new_unsafe in lib.c");
		return;
	}
	return io;
}

wchar_t* IO_buffer_to_string_unsafe(IO* io) {
	size_t total_byte_length = io->output_buffer->size * 4 + 1;
	wchar_t* result = (wchar_t*)malloc(total_byte_length * sizeof(wchar_t));
	if (result == NULL) {
		error_append_format("Failed to allocate memory for IO_buffer_to_string\n\tIO_buffer_to_string_unsafe in lib.c");
		return;
	}

	for (size_t i = 0; i < io->output_buffer->size; i++) {
		result[i] = (wchar_t)io->output_buffer->data[i];
	}
	result[io->output_buffer->size] = L'\0';

	return result;
}

wchar_t IO_output_unsafe(IO* io, Memory* runtime_memory) {
	uint32_t n = Memory_output(runtime_memory);
	if (n > 0x10FFFF || (n >= 0xD800 && n <= 0xDFFF)) {
		error_append_format("Invalid Unicode scalar value: %" PRIu32 "\n\tIO_output_unsafe in lib.c", n);
		return;
	}

	if (io->output_buffer != NULL) {
		vector_push_unsafe(io->output_buffer, n);
		if (error_check()) {
			error_append_format("\n\tIO_output_unsafe in lib.c", n);
		}
	}

	return (wchar_t)n;
}

void IO_input_unsafe(IO* io, Memory* runtime_memory) {
	printf("Input:");
	wchar_t buffer[input_buffer_length + 1];
	fgetws(buffer, input_buffer_length, stdin);
	uint32_t n = input_to_u32_unsafe(buffer);
	if (error_check()) {
		error_append_format("\n\tIO_input_unsafe in lib.c");
		return;
	}

	if (n > runtime_memory->cell_max) {
		error_append_format("Input value %" PRIu32 " exceeds the maximum cell value %" PRIu32 "\n\tIO_input_unsafe in lib.c", n, runtime_memory->cell_max);
		return;
	}

	Memory_input(runtime_memory, n);
}

void IO_free(IO* io) {
	if (io != NULL) {
		vector_free(io->output_buffer);
		free(io);
	}
}

//////////////////// UTIL  ////////////////////

static wchar_t* ltrim(wchar_t* str) {
	while (*str && iswspace(*str)) {
		str++;
	}
	return str;
}

static wchar_t* rtrim(wchar_t* str) {
	wchar_t* end = str + wcslen(str) - 1;
	while (end >= str && iswspace(*end)) {
		*end = L'\0';
		end--;
	}
	return str;
}

static wchar_t* trim(wchar_t* str) {
	return ltrim(rtrim(str));
}

static bool ends_with_u32(const wchar_t* str) {
	size_t len = wcslen(str);
	if (len == 0 || len <= 3) {
		return false;
	}
	return str[len - 1] == L'2' && str[len - 2] == L'3' && str[len - 3] == L'u';
}

static uint32_t input_to_u32_unsafe(wchar_t* buffer) {
	if (wcsncmp(buffer, L"\n", input_buffer_length) == 0 || wcsncmp(buffer, L"\r\n", input_buffer_length) == 0) {
		return 10;
	}
	wchar_t* trimmed = trim(buffer);
	size_t len = wcslen(trimmed);
	if (ends_with_u32(trimmed)) {
		trimmed[len - 3] = L'\0';
		wchar_t* end_ptr;
		uint32_t value = (uint32_t)wcstoul(trimmed, &end_ptr, 10);
		if (errno == ERANGE && (value == ULONG_MAX || value == 0)) { // 数值超出范围
			error_append_format("Parse error: number too large to fit in target type\n\tinput_to_u32_unsafe in lib.c");
			return;
		}
		else if (end_ptr == trimmed) { // 没有有效的转换
			error_append_format("Parse error: invalid digit found in string\n\tinput_to_u32_unsafe in lib.c");
			return;

		}
		else if (*end_ptr != L'\0') { // 转换后有剩余字符
			error_append_format("Parse error: invalid digit found in string\n\tinput_to_u32_unsafe in lib.c");
			return;
		}
		return value;
	}


	if (len != 1) {
		error_append_format("The length of the input string is greater than 1, unable to parse into a char\n\tinput_to_u32_unsafe in lib.c");
		return;
	}

	return (uint32_t)trimmed[0];
}


void show_ir(Token_vec* token_vec) {
	for (size_t i = 0; i < token_vec->size; i++) {
		Token token = token_vec->data[i];

		switch (token.type)
		{
		case PTR_INCREASE: {
			printf("PTR_INCREASE   %" PRIu32 "\n", token.value);
			break;
		}
		case PTR_DECREASE: {
			printf("PTR_DECREASE   %" PRIu32 "\n", token.value);
			break;
		}
		case DATA_INCREASE: {
			printf("DATA_INCREASE  %" PRIu32 "\n", token.value);
			break;
		}
		case DATA_DECREASE: {
			printf("DATA_DECREASE  %" PRIu32 "\n", token.value);
			break;
		}
		case JUMP_FORWARD: {
			printf("JUMP_FORWARD   %" PRIu32 "\n", token.value);
			break;
		}
		case JUMP_BACK: {
			printf("JUMP_BACK      %" PRIu32 "\n", token.value);
			break;
		}
		case OUTPUT: {
			printf("OUTPUT\n");
			break;
		}
		case INPUT: {
			printf("INPUT\n");
			break;
		}
		}

	}
}