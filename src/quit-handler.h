#pragma once

void register_signal_handlers();
int get_final_exit_code();
void kill_all_quit();
void set_return_code(int code);