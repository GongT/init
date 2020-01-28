#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include "process-create.h"

int code = -1;
bool process_is_terminate = false;

int get_final_exit_code()
{
	return code >= 0 ? code : EXIT_FAILURE;
}

void set_return_code(int new_code)
{
	if (code == -1)
		code = new_code;
}

void signal_handler(int sig)
{
	set_return_code(code);
	printf_stderr("[process signal] receive %d.\n", sig);
	if (process_is_terminate)
		printf_stderr("[process signal] please wait.\n");
	else
		kill_all_quit();
}

void register_ctrl_c()
{
	struct sigaction sa = {0};
	sa.sa_handler = signal_handler;
	sigaction(SIGINT, &sa, NULL);
}

void kill_all_quit()
{
	if (process_is_terminate)
		return;
	process_is_terminate = true;
	printf_stderr("kill_all_quit()\n");

	FOREACH_PROCESS(ptr, index)
	{
		if (ptr->quit)
			continue;

		if (kill(ptr->pid, ptr->source->signal) != 0)
		{
			printf_stderr("failed kill process %u: (%d)%s\n", ptr->pid, (int)errno, (char *)strerror(errno));
			kill(ptr->pid, 9);
		}
		else
		{
			printf_stderr("send signal %d to process %u\n", ptr->source->signal, ptr->pid);
		}
	}

	printf_stderr("kill_all_quit() finish, wait cleanup.\n");
}
