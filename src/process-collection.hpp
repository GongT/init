#pragma once

#include <list>
#include "process-create.hpp"

class ProcessCollection
{
private:
	bool is_terminating;
	bool this_is_first_quit;
	std::list<ProcessHandle> processes;

public:
	ProcessCollection();
	void killAll();
	void wait();
	bool run(const Program *program);
};

extern ProcessCollection processCollection;
