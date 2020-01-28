#pragma once

#include "my-types.h"

extern process_handle_t **processes;
extern unsigned int process_count;
extern bool process_is_terminate;
void run_all(program_list_t programs);
void kill_all_quit();

#define FOREACH_PROCESS(varname, index)       \
	process_handle_t *varname = processes[0]; \
	for (unsigned int index = 0; index < process_count; index++, varname = processes[index])
