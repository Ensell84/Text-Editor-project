#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

#define KEY_CTRL(c) (c & 0x1f)

struct editor_state {
    int screenrows;
    int screencols;
    struct termios original_termios;
};

struct editor_state E;

/***Miscellaneous***/

void clear_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void error_handler(const char* s) {
    clear_screen();

    perror(s);
    exit(1);
}

/***terminal***/

void disable_terminal_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.original_termios) == -1) 
        error_handler("tcsetattr at disable_terminal_raw_mode()");
}

void enable_terminal_raw_mode() {
    // save original terminal termios structure to 
    // return terminal to original state, when we
    // finish execution of our program
    if (tcgetattr(STDIN_FILENO, &E.original_termios) == -1)
        error_handler("tcgetattr at enable_terminal_raw_mode()");
    atexit(disable_terminal_raw_mode);

    struct termios raw = E.original_termios;
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

int get_window_size(int* rows, int* cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }
    else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
        return 0;
    }
}

/***input***/

// Function that deals with high-level input recieved from 
// editor_read_key() function that takes low-level job.
void editor_process_keypress() {
    char c = editor_read_key();
    
    switch (c) {
        case KEY_CTRL('q'):
        {
            clear_screen();
            exit(0);
            break;
        }
    }
}

/***rendering***/

void editor_render_rows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "~", 1);
        if (y < E.screenrows - 1) {
            write(STDOUT_FILENO, "\r\n", 2);
        }
    } 
}

void editor_render_screen() {
    clear_screen();

    editor_render_rows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/***init***/

void init_editor() {
    if (get_window_size(&E.screenrows, &E.screencols) == -1) 
        error_handler("get_window_size() in init_editor()");
}

int main() {
    enable_terminal_raw_mode();
    init_editor();
    
    editor_render_screen();
    for(;;) {
        editor_process_keypress();
        editor_render_screen();
    }
   
    return 0;
}
