#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE


#include "terminal.h"
#include "global_structs.h"
#include "rows_operation.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void error_handler(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disable_terminal_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &editorData.original_termios) == -1) 
        error_handler("tcsetattr at disable_terminal_raw_mode()");
}

void enable_terminal_raw_mode() {
    // save original terminal termios structure to
    // return terminal to original state, when we
    // finish execution of our program
    if (tcgetattr(STDIN_FILENO, &editorData.original_termios) == -1)
        error_handler("tcgetattr at enable_terminal_raw_mode()");
    atexit(disable_terminal_raw_mode);

    struct termios raw = editorData.original_termios;
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
int editor_read_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1){
        if(nread == -1 && errno != EAGAIN) error_handler("read() at editor_read_key()");
    }
    
    // detecting Escape Sequences:
    if (c == '\x1b') {
        char sequence[3];
        if(read(STDIN_FILENO, &sequence[0], 1) != 1) return '\x1b';
        if(read(STDIN_FILENO, &sequence[1], 1) != 1) return '\x1b';
        if (sequence[0] == '[') {
            if(sequence[1] >= 0 && sequence[1] <= 9) {
                if(read(STDIN_FILENO, &sequence[2], 1) != 1) return '\x1b';
                if(sequence[2] == '~') {
                    // ESC [ x ~
                    switch (sequence[1]) {
                        case 1: return HOME_KEY;
                        case 3: return DELETE_KEY;
                        case 4: return END_KEY;
                        case 5: return PAGE_UP;
                        case 6: return PAGE_DOWN;
                        case 7: return HOME_KEY;
                        case 8: return END_KEY;
                    }
                }
            } else {
                // ESC [ X
                switch (sequence[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if(sequence[0] == 'O') {
            // ESC O X
            switch (sequence[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            } 
        }
        return '\x1b';
    } else {
        return c;
    }
    return c;
}

int get_window_size(int* rows, int* cols) {
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
        return 0;
    }
}

void editor_open(char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) error_handler("fopen() at editor_open()");

    char* line = NULL;
    size_t linecapp = 0;
    ssize_t linelength;

    while ((linelength = getline(&line, &linecapp, f)) != -1) {
        while (linelength > 0 && line[linelength - 1] == '\n' || line[linelength - 1] == '\r') {
            linelength--;
        }
        editor_append_row(line, linelength);
    }
    
    free(line);
    fclose(f);
}

char* editor_rows_to_string(int* buflen) {
    int totallen = 0;
    for (int i = 0; i < editorData.numrows; i++) {
        totallen += editorData.rows[i].size + 1; 
    }
    *buflen = totallen;

    char* buffer = malloc(totallen);
    char* buf_point = buffer;
    for (int i = 0; i < editorData.numrows; i++) {
        memcpy(buf_point, editorData.rows[i].row, editorData.rows[i].size); 
        buf_point += editorData.rows[i].size;

        *buf_point = '\n';
        buf_point++;
    }
    return buffer;
}

void editor_save() {
    if (editorData.filename == NULL) return;
    
    int length;
    char* buffer = editor_rows_to_string(&length);
    
    int fd = open(editorData.filename, O_RDWR | O_CREAT, 0644);

    // 0644 - standart permission (creator - can read/write, others - can read)
    if (fd != -1) {
        if (ftruncate(fd, length) != -1) {
            // usually O_TRUNC - used, but ftruncate - safer
            if (write(fd, buffer, length) == length) {
                close(fd);
                free(buffer);
                return;
            }
        }
        close(fd);
    }

    free(buffer);
}
