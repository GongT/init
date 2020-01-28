#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include "my-types.h"
#include "passthru-output.h"
#include "process-create.h"

process_handle_t **processes = NULL;
unsigned int process_count = 0;

void kill_all_quit()
{
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

process_handle_t *do_fork(char *const *argv)
{
	int out[2] = {-1, -1};
	if (pipe(out) == -1)
	{
		printf_stderr("failed open stdout.");
		kill_all_quit();
	}
	int err[2] = {-1, -1};
	if (pipe(err) == -1)
	{
		printf_stderr("failed open stderr.");
		kill_all_quit();
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		printf_stderr("fork() fail.");
		kill_all_quit();
	}
	if (pid == 0)
	{
		close(out[OUTPUT_END]);
		dup2(out[INPUT_END], STDOUT_FILENO);
		close(out[INPUT_END]);

		close(err[OUTPUT_END]);
		dup2(err[INPUT_END], STDERR_FILENO);
		close(err[INPUT_END]);

		errno = 0;
		execv(argv[0], argv);
		printf_stderr("execl() fail %d: %s.\n", errno, strerror(errno));
		exit(233);
	}
	else
	{
		close(out[INPUT_END]);
		close(err[INPUT_END]);

		process_handle_t rt = {
			.pid = pid,
			.fd0 = out[OUTPUT_END],
			.fd1 = err[OUTPUT_END],
			.source = NULL,
			.quit = 0,
		};
		return ALLOC_STRUCT(rt);
	}
}

void run_all(program_list_t programs)
{
	processes = malloc((programs.size + 1) * sizeof(void *));
	memset(processes, 0, (programs.size + 1) * sizeof(void *));
	for (unsigned int index = 0; index < programs.size; index++)
	{
		program_t *p = programs.head[index];

		printf("running program %s...\n", p->title);
		fputs(" +", stdout);
		for (int i = 0; p->command[i] != NULL; i++)
		{
			printf(" %s", p->command[i]);
		}
		puts("");

		process_handle_t *handle = do_fork(p->command);
		printf("pid is %d\n", handle->pid);
		handle->source = p;

		processes[index] = handle;
		process_count++;

		char *ptitle = smalloc(sizeof(char) * (strlen(p->title) + 3));
		sprintf(ptitle, "[%s] ", p->title);

		if (!watch_process_output(ptitle, handle->fd0, STDOUT_FILENO) || !watch_process_output(ptitle, handle->fd1, STDERR_FILENO))
			return;
	}
}
