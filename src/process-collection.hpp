#pragma once

#include <list>
#include "process-create.hpp"

class ProcessCollection
{
private:
	bool is_terminating;
	std::list<ProcessHandle> processes;

public:
	ProcessCollection();
	void killAll();
	void wait();
	void run(const Program *program);
};

extern ProcessCollection processCollection;
