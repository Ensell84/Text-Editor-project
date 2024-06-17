#ifndef TERMINAL_H
#define TERMINAL_H

void enable_terminal_raw_mode();
void disable_terminal_raw_mode();
int editor_read_key();
void error_handler(const char* s);
int get_window_size(int* rows, int* cols);

#endif
