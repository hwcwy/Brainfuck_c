#pragma once

#include "vector.h"
#include "type.h"

Memory* Memory_new_unsafe(uint32_t cell_max);

void Memory_display(Memory* memory);

void Memory_ptr_increase_unsafe(Memory* memory, uint32_t n);

void Memory_ptr_decrease_unsafe(Memory* memory, uint32_t n);

void Memory_data_increase(Memory* memory, uint32_t n);

void Memory_data_decrease(Memory* memory, uint32_t n);

uint32_t Memory_output(Memory* memory);

void Memory_input(Memory* memory, uint32_t n);

void Memory_free(Memory* memory);

IO* IO_new_unsafe(Output_mode output_mode,bool with_buffer);

wchar_t* IO_buffer_to_string_unsafe(IO* io);

wchar_t IO_output_unsafe(IO* io, Memory* runtime_memory);

#define input_buffer_length (20)

void IO_input_unsafe(IO* io, Memory* runtime_memory);

void IO_free(IO* io);

static wchar_t* ltrim(wchar_t* str);

static wchar_t* rtrim(wchar_t* str);

static wchar_t* trim(wchar_t* str);

static bool ends_with_u32(const wchar_t* str);

static uint32_t input_to_u32_unsafe(wchar_t* buffer);

void show_ir(Token_vec* token_vec);