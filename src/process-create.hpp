#pragma once

#include <list>
#include "file-parser.hpp"

class ProcessHandle
{
protected:
	pid_t _pid;
	int _fd0;
	int _fd1;
	const Program *_source;
	volatile bool _quit;
	std::list<ProcessHandle> _processes;

public:
	ProcessHandle(const Program *source);

	const string debug() const { return "[" + this->_source->title() + "] pid=" + std::to_string(this->_pid) + " "; };
	const pid_t &pid() const { return this->_pid; };
	const int &fd0() const { return this->_fd0; };
	const int &fd1() const { return this->_fd1; };
	const Program &source() const { return *this->_source; };
	bool quit() const { return this->_quit; };

	bool kill(int s);
	bool kill();

	void notifyQuit() { _quit = true; }
};
