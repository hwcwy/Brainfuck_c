#pragma once

#include <stdint.h>
#include "type.h"

void println_token_type(Token* token);

Token_vec* raw_code_to_token_vec_unsafe(char* raw_code);