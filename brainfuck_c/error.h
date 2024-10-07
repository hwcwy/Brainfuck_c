#pragma once
#include <stdbool.h>

extern char *global_error_msg;
extern bool global_error_flag;

void error_resolve();
bool error_check();
void error_append_format(const char *format, ...);