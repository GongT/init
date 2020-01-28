#pragma once

void create_debug_output();
void shutdown_debug_output();
bool watch_process_output(const char *prefix, const int source, const int target);
bool unwatch_process_output(const int source);
