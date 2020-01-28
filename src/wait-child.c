#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process-create.h"
#include "passthru-output.h"

void wait_all_child_exit()
{
	static int some_quit = 0;
	while (true)
	{
		int status = 0;
		size_t running = 0;
		pid_t pid = waitpid(-1, &status, 0);
		printf_stderr("[process quit] %u (status=%d).\n", pid, status);

		FOREACH_PROCESS(ptr, index)
		{
			if (ptr->pid == pid)
			{
				ptr->quit = 1;
				unwatch_process_output(ptr->fd0);
				unwatch_process_output(ptr->fd1);
			}
			if (!ptr->quit)
				running++;
		}

		printf_stderr("[process quit] %d process remaining.\n", running);
		if (running == 0)
		{
			printf_stderr("[process quit] shutdown.\n");
			return;
		}

		if (!some_quit)
		{
			some_quit = 1;
			kill_all_quit();
		}
	}
}