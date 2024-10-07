#include "utils.h"
#include <ctype.h>
#include <string.h>

void trim_end(char* str) {
	int len = strlen(str);
	while (len > 0 && isspace((unsigned char)str[len - 1])) {
		str[--len] = '\0';
	}
}

bool cmp(const char* str1, const char* str2)
{
	return strcmp(str1, str2) == 0;
}