#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	PTR_INCREASE,  // 指针增加
	PTR_DECREASE,  // 指针减少
	DATA_INCREASE, // 数据增加
	DATA_DECREASE, // 数据减少
	JUMP_FORWARD,  // 向前跳转
	JUMP_BACK,     // 向后跳转
	OUTPUT,        // 输出
	INPUT          // 输入
} Token_type;


typedef struct {
	size_t size;
	size_t capacity;
	uint32_t* data;
} Vector;

typedef struct {
	Token_type type;
	uint32_t value;
} Token;

typedef struct {
	size_t size;
	size_t capacity;
	Token* data;
} Token_vec;

typedef enum {
	Individually,
	Bulk,
} Output_mode;

typedef struct {
	char* raw_code;
	Output_mode output_mode;
	Token_vec* token_vec;
	uint32_t cell_max;
	bool verbose;
	bool repl_mode;
	bool show_ir;
} Config;

typedef struct {
	uint32_t ptr;
	uint32_t cell_max;
	Vector* view;
} Memory;


typedef struct {
	Vector* output_buffer;
	Output_mode output_mode;
} IO;

typedef struct {
	Token_vec* view;
	uint32_t ptr;
} Exec_queue;