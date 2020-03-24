#include <iostream>
#include <sys/wait.h>
#include "process-collection.hpp"
#include "quit-handler.hpp"
#include "passthru-output.hpp"

ProcessCollection processCollection;

ProcessCollection::ProcessCollection() {}

bool ProcessCollection::run(const Program *program)
{
	std::cerr << "running program " << program->title() << std::endl
			  << " +";
	auto cmds = program->commands();
	for (auto s = cmds.begin(); s != cmds.end(); s++)
	{
		std::cerr << ' ' << *s;
	}
	std::cerr << std::endl;

	processes.push_back(ProcessHandle(program));
	ProcessHandle *handle = &processes.back();
	std::cerr << "pid is " << handle->pid() << std::endl;

	return outputCollector.enable(handle);
}

void ProcessCollection::killAll()
{
	if (is_terminating)
		return;
	is_terminating = true;
	std::cerr << "ProcessCollection::killAll()" << std::endl;

	for (auto ptr = processes.begin(); ptr != processes.end(); ptr++)
	{
		if (ptr->quit())
			continue;

		if (ptr->kill())
		{
			std::cerr << "send signal " << ptr->source().signal() << " to process " << ptr->pid() << std::endl;
		}
		else
		{
			ERROR_LOG("failed kill process " << ptr->pid());
			ptr->kill(9);
		}
	}

	std::cerr << "kill_all_quit() finish, wait cleanup." << std::endl;
}

void ProcessCollection::wait()
{
	static int some_quit = 0;
	while (true)
	{
		int status = 0;
		// cerr << "(process quit) waiting any child change state..." << std::endl;
		pid_t pid = waitpid(-1, &status, 0);

		if (pid < 0)
		{
			cerr << "(process quit) wait return, but failed. ";
			printf_last_error();
			cerr << std::endl;
			continue;
		}

		unsigned int code = WEXITSTATUS(status); // waitpid only return exit by default, otherwise need a check

		bool is_my_child = false;
		for (auto ptr = processes.begin(); ptr != processes.end(); ptr++)
		{
			if (ptr->pid() == pid)
			{
				cerr << "(process quit) " << ptr->pid() << " == " << ptr->source().title() << " == isQuit=" << ptr->quit() << "." << std::endl;
				ptr->notifyQuit();
				outputCollector.disable(&(*ptr));
				processes.erase(ptr);
				is_my_child = true;
				break;
			}
		}
		if (!is_my_child)
			continue;

		std::cerr << "(process quit) " << pid << ", status=" << status << " (code=" << code << ")." << std::endl;

		if (code != 0)
			set_return_code(code);

		if (processes.size() == 0)
		{
			std::cerr << std::endl
					  << "(process quit) no process remaining, shutdown." << std::endl;
			return;
		}
		else
		{
			std::cerr << "(process quit)     " << processes.size() << " process remaining." << std::endl;
		}

		if (!some_quit)
		{
			some_quit = 1;
			killAll();
		}
	}
}
