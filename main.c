#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios original_termios;

void disable_terminal_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_terminal_raw_mode() {
    // save original terminal termios structure to 
    // return terminal to original state, when we
    // finish execution of our program
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_terminal_raw_mode);

    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    char c;
    enable_terminal_raw_mode();
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        write(STDOUT_FILENO, &c, 1);        
    }
    return 0;
}
