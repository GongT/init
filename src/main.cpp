#include <iostream>
#include <unistd.h>
#include "quit-handler.hpp"
#include "passthru-output.hpp"
#include "file-parser.hpp"
#include "process-collection.hpp"

using namespace std;

const char *CONFIGFILE = "/etc/programs";

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		// do nothing
	}
	else if (argc == 2)
	{
		CONFIGFILE = argv[1];
	}
	else
	{
		std::cerr << "this init allow at most 1 argument, got " << argc << "." << std::endl;
		for (char **arg = argv; *arg != 0; arg++)
		{
			std::cerr << " * " << *arg << std::endl;
		}
		return 233;
	}
	std::cerr << "=====================" << std::endl;
	for (char **env = environ; *env != 0; env++)
	{
		std::cerr << *env << std::endl;
	}
	std::cerr << "=====================" << std::endl;

	close(STDIN_FILENO);
	register_signal_handlers();

	pid_t pid = getpid();
	std::cerr << "(main) hello world." << std::endl;
	std::cerr << "(main) pid: " << pid << std::endl;

	outputCollector.startup();

	const vector<const Program *> programs = Program::ParseFile(CONFIGFILE);

	if (programs.size() == 0)
		die("No program defined in '%s'", CONFIGFILE);

	for (auto ptr = programs.begin(); ptr != programs.end(); ptr++)
	{
		if (!processCollection.run(*ptr))
		{
			std::cerr << "(main)  process spawn fail." << std::endl;
			break;
		}
		std::cerr << "(main)  process spawn ok" << std::endl;
	}
	std::cerr << "(main) All process spawn complete." << std::endl;

	processCollection.wait();

	std::cerr << "(main) Wait has return." << std::endl;

	outputCollector.shutdown();

	int r = get_final_exit_code();
	std::cerr << "(main) end with code: " << r << std::endl;
	return r;
}
