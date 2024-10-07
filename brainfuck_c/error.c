#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char* global_error_msg = NULL;
bool global_error_flag = false;

void error_resolve() {
	global_error_flag = false;
}

bool error_check() {
	return global_error_flag;
}
void error_append_format(const char* format, ...) {
    global_error_flag = true;
    if (global_error_msg == NULL) {
        global_error_msg = malloc(1);
        if (global_error_msg == NULL) {
            perror("malloc failed\n\terror_append_format in error.c");
            exit(EXIT_FAILURE);
        }
        global_error_msg[0] = '\0';
    } 

    va_list args;
    va_start(args, format);

    // 计算格式化字符串的长度
    int len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (len < 0) {
        perror("vsnprintf failed\n\terror_append_format in error.c");
        exit(EXIT_FAILURE);
    }

    // 重新分配内存
    size_t new_len = (size_t)strlen(global_error_msg) + (size_t)len + 1;
    char* new_str = realloc(global_error_msg, new_len);
    if (new_str == NULL) {
        perror("realloc failed\n\terror_append_format in error.c");
        exit(EXIT_FAILURE);
    }
    global_error_msg = new_str;

    // 追加格式化字符串
    va_start(args, format);
    vsnprintf(global_error_msg + strlen(global_error_msg), (size_t)len + 1, format, args);
    va_end(args);
}