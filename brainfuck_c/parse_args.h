#pragma once

#include "lib.h"
#include "type.h"

extern const char* const HELP;

static Config* config_new_unsafe();

Config* config_from_args_unsafe(int arg_c, char* args[]);

static char* read_file_unsafe(const char* filename);