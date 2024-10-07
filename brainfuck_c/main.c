#include "error.h"
#include "type.h"
#include "lib.h"
#include "token.h"
#include "parse_args.h"
#include "interpreter.h"

#include <locale.h>
#include <stdio.h>


static void run(Config* config) {
	if (!config->repl_mode) {
		config->token_vec = raw_code_to_token_vec_unsafe(config->raw_code);
		if (error_check()) return error_append_format("\n\trun in main.c");
	}

	if (config->show_ir) show_ir(config->token_vec);
	else interpreter_run(config);

	if (error_check()) return error_append_format("\n\trun in main.c");
}


int main(int arg_c, char* args[]) {
	setlocale(LC_ALL, "zh_CN.UTF-8");
	Config* config = config_from_args_unsafe(arg_c, args);

	if (error_check()) {
		fprintf(stderr,"\n%s\n\tmain in main.c", global_error_msg);
		return;
	}

	run(config);

	if (error_check()) {
		fprintf(stderr, "\n%s\n\tmain in main.c", global_error_msg);
		return;
	}
}