#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <memory>
#include "process-create.hpp"
#include "process-collection.hpp"

using namespace std;

ProcessHandle::ProcessHandle(const Program *source)
	: _source(source)
{
	int out[2] = {-1, -1};
	if (pipe(out) == -1)
	{
		cerr << "failed open stdout." << endl;
		processCollection.killAll();
	}
	int err[2] = {-1, -1};
	if (pipe(err) == -1)
	{
		cerr << "failed open stderr." << endl;
		processCollection.killAll();
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		cerr << "fork() fail." << endl;
		processCollection.killAll();
	}
	if (pid == 0)
	{
		close(STDIN_FILENO);

		close(out[OUTPUT_END]);
		dup2(out[INPUT_END], STDOUT_FILENO);
		close(out[INPUT_END]);

		close(err[OUTPUT_END]);
		dup2(err[INPUT_END], STDERR_FILENO);
		close(err[INPUT_END]);

		auto cmds = source->commands();
		auto size = cmds.size();
		char **argv = (char **)calloc(size + 1, sizeof(char *));
		for (size_t itr = 0; itr < size; itr++)
		{
			argv[itr] = (char *)calloc(cmds[itr].size() + 1, sizeof(char));
			memcpy(argv[itr], cmds[itr].c_str(), cmds[itr].size());
			// cerr << itr << ": " << argv[itr] << endl;
		}

		execv(argv[0], argv);
		cerr << "execv() fail " << errno << ": " << strerror(errno) << "." << endl;
		exit(233);
	}
	else
	{
		close(out[INPUT_END]);
		close(err[INPUT_END]);

		_pid = pid;
		_fd0 = out[OUTPUT_END];
		_fd1 = err[OUTPUT_END];
		_quit = false;
	}
}

bool ProcessHandle::kill()
{
	return ::kill(_pid, _source->signal()) == 0;
}

bool ProcessHandle::kill(int signal)
{
	return ::kill(_pid, signal) == 0;
}
