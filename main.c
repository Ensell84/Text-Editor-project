#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>

struct termios original_termios;

void error_handler(const char* s) {
    perror(s);
    exit(1);
}

void disable_terminal_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) 
        error_handler("tcsetattr at disable_terminal_raw_mode()");
}

void enable_terminal_raw_mode() {
    // save original terminal termios structure to 
    // return terminal to original state, when we
    // finish execution of our program
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
        error_handler("tcgetattr at enable_terminal_raw_mode()");
    atexit(disable_terminal_raw_mode);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // Miscellaneous flags for compatability:
    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP);
    raw.c_cflag |= ~(CS8);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        error_handler("tcsetattr at enable_terminal_raw_mode()");
}

int main() {
    enable_terminal_raw_mode();
    
    for(;;) {
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
            error_handler("read() at main cycle");

        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
       if (c == 'q') break; 
    }
   
    return 0;
}
