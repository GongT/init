#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "my-types.h"
#include "file-parser.h"
#include "process-create.h"
#include "passthru-output.h"
#include "wait-child.h"

const char *CONFIGFILE = "/etc/programs";

void signal_handler(int sig)
{
	printf_stderr("[process signal] receive %d.\n", sig);
	kill_all_quit();
}
int main()
{
	close(STDIN_FILENO);
	printf_stderr("hello world.\n");

	create_debug_output();

	program_list_t programs = parse_file(CONFIGFILE);

	if (programs.size == 0)
		die("No program defined in '%s'", CONFIGFILE);

	struct sigaction sa = {0};
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);

	run_all(programs);

	wait_all_child_exit();

	shutdown_debug_output();

	return 1;
}
