#include <stdio.h>
#include <unistd.h>
#include "my-types.h"
#include "file-parser.h"
#include "process-create.h"
#include "passthru-output.h"
#include "wait-child.h"
#include "quit-handler.h"

const char *CONFIGFILE = "/etc/programs";

int main()
{
	close(STDIN_FILENO);
	register_ctrl_c();

	printf_stderr("hello world.\n");

	create_debug_output();

	program_list_t programs = parse_file(CONFIGFILE);

	if (programs.size == 0)
		die("No program defined in '%s'", CONFIGFILE);

	run_all(programs);

	wait_all_child_exit();

	shutdown_debug_output();

	int r = get_final_exit_code();
	printf_stderr("init end with code %d\n", r);
	return r;
}
