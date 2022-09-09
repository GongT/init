#include <cstdlib>
#include <csignal>
#include <iostream>
#include "process-collection.hpp"

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
	static uint8_t retry_kill = 0;
	set_return_code(code);
	std::cerr << "[process signal] init process receive signal" << sig << "." << std::endl;
	if (process_is_terminate)
	{
		std::cerr << "[process signal] shutdown is in progress." << std::endl;
		if (retry_kill >= 3)
		{
			std::cerr << "[process signal] got Ctrl+C more than 3 times, program will instant die!!" << std::endl;
			set_return_code(9);
			_Exit(9);
		}
		retry_kill++;
	}
	else
		processCollection.killAll();
}

void test_handler(int sig)
{
	std::cerr << "[process signal] (test) receive SIGUSR2(" << sig << ")." << std::endl;
}

void register_signal_handlers()
{
	struct sigaction sa;
	sa.sa_handler = &signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	struct sigaction sa2;
	sa2.sa_handler = &test_handler;
	sigemptyset(&sa2.sa_mask);
	sa2.sa_flags = 0;
	sigaction(SIGUSR2, &sa2, NULL);
}
