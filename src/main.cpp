#include <iostream>
#include <unistd.h>
#include "quit-handler.hpp"
#include "passthru-output.hpp"
#include "file-parser.hpp"
#include "process-collection.hpp"

using namespace std;

const char *CONFIGFILE = "/etc/programs";

int main()
{
	close(STDIN_FILENO);
	register_signal_handlers();

	pid_t pid = getpid();
	printf("pid: %d\n", pid);
	cerr << "hello world." << endl;

	outputCollector.startup();

	const vector<Program> programs = Program::ParseFile(CONFIGFILE);

	if (programs.size() == 0)
		die("No program defined in '%s'", CONFIGFILE);

	for (auto ptr = programs.begin(); ptr != programs.end(); ptr++)
	{
		processCollection.run(*ptr);
	}

	processCollection.wait();

	outputCollector.shutdown();

	int r = get_final_exit_code();
	cout << "init end with code " << r << endl;
	return r;
}
