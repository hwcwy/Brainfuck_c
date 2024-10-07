#include "parse_args.h"
#include "error.h"
#include "lib.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char* const HELP =
"Usage:\n"
"\n"
"[code]              Use an argument as the code\n"
"-f [path]           Read code from a file\n"
"--REPL | --repl     Start in REPL mode\n"
"--bulk              Bulk output mode\n"
"--cell [u8|u16|u32] Set the cell size\n"
"-v | --verbose      Display verbose information\n"
"--IR                Display intermediate representation of the code\n"
"[input]             Input can be a string ending with 'u32' to be parsed as uint32";



static Config* config_new_unsafe() {
	Config* config = malloc(sizeof(Config));
	if (config == NULL) {
		error_append_format("Failed to allocate memory for Config\n\tconfig_new_unsafe in parse_args.c");
		return;
	}

	config->raw_code = NULL;
	config->output_mode = Individually;
	config->token_vec = token_vec_new_unsafe();
	if (error_check()) {
		error_append_format("\n\tconfig_new_unsafe in parse_args.c");
		return;
	}
	config->cell_max = 255;
	config->verbose = false;
	config->repl_mode = false;
	config->show_ir = false;
	return config;
}

Config* config_from_args_unsafe(int arg_c, char* args[]) {
	Config* config = config_new_unsafe();
	if (error_check()) {
		error_append_format("\n\tconfig_from_args_unsafe in parse_args.c");
		return;
	}

	for (int i = 1; i < arg_c; i++) {
		char* arg = args[i];

		if (cmp(arg, "-h") || cmp(arg, "-H") || cmp(arg, "--help")) {
			printf("%s", HELP);
			exit(0);
		}
		else if (cmp(arg, "-f")) {
			if (i + 1 < arg_c) {
				char* file_path = args[++i];
				config->raw_code = read_file_unsafe(file_path);
				if (error_check()) {
					error_append_format("\n\tconfig_from_args_unsafe in parse_args.c");
					return;
				}
			}
			else {
				printf("File path not specified");
				exit(1);
			}
		}
		else if (cmp(arg, "-v") || cmp(arg, "--verbose")) {
			config->verbose = true;
		}
		else if (cmp(arg, "--bulk")) {
			config->output_mode = Bulk;
		}
		else if (cmp(arg, "--cell")) {
			if (i + 1 < arg_c) {
				char* cell_size_type = args[++i];
				if (cmp(cell_size_type, "u8")) {
					config->cell_max = 255;
				}
				else if (cmp(cell_size_type, "u16")) {
					config->cell_max = 65535;
				}
				else if (cmp(cell_size_type, "u32")) {
					config->cell_max = 4294967295;
				}
				else {
					printf("Invalid cell size type %s", cell_size_type);
					exit(1);
				}
			}
			else {
				printf("Cell size type not specified");
				exit(1);
			}
		}
		else if (cmp(arg, "--REPL") || cmp(arg, "--repl")) {
			config->repl_mode = true;
		}
		else if (cmp(arg, "--IR")) {
			config->show_ir = 1;
		}
		else {
			if (arg[0] == '-') {
				printf("Unknown argument \"%s\"\n", arg);
				printf("%s\n", HELP);
				exit(1);
			}
			if (config->raw_code == NULL) {
				config->raw_code = arg;
			}
		}
	}

	if (config->raw_code == NULL && !config->repl_mode) {
		printf("No code found");
		exit(1);
	}
	return config;
}

char* read_file_unsafe(const char* filename) {
	// 打开文件
	FILE* file = fopen(filename, "r");
	if (!file) {
		error_append_format("File opening failed\n\tread_file in parse_args.c");
		return;
	}

	// 移动文件指针到文件末尾
	fseek(file, 0, SEEK_END);
	// 获取文件大小
	long file_size = ftell(file);
	// 将文件指针移回文件开头
	rewind(file);

	// 为文件内容分配内存 (+1是为了终止符 '\0')
	char* content = (char*)malloc((size_t)file_size + 1);
	if (!content) {
		error_append_format("Memory allocation failed for file content\n\tread_file in parse_args.c");
		fclose(file);
		return;
	}

	// 读取文件内容
	size_t read_size = fread(content, 1, file_size, file);
	content[read_size] = '\0';  // 添加字符串终止符

	// 关闭文件
	fclose(file);

	return content;
}

