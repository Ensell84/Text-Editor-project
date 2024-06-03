#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>

#define KEY_CTRL(c) (c & 0x1f)

struct termios original_termios;

void error_handler(const char* s) {
    perror(s);
    exit(1);
}

/***terminal***/

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
    
    // flags for read() function:
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        error_handler("tcsetattr at enable_terminal_raw_mode()");
}

// Low-level function that deals with low level terminal input
char editor_read_key() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if(nread == -1 && errno != EAGAIN) error_handler("read() at editor_read_key()");
    }
    return c;
}

/***input***/

// Function that deals with high-level input recieved from 
// editor_read_key() function that takes low-level job.
void editor_process_keypress() {
    char c = editor_read_key();
    
    switch (c) {
        case KEY_CTRL('q'):
        {
            exit(0);
            break;
        }
    }
}

/***init***/

int main() {
    enable_terminal_raw_mode();
    
    for(;;) {
        editor_process_keypress();
    }
   
    return 0;
}
