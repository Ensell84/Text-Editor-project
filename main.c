#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

#define KEY_CTRL(c) (c & 0x1f)

enum editorKey {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN, 
    DELETE_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

typedef struct {
    int cx, cy;
    int screenrows;
    int screencols;
    struct termios original_termios;
}editorState;

editorState editorData;

/*** Miscellaneous ***/

void clear_screen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void error_handler(const char* s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

/*** terminal ***/

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
            }
            else {
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
        }
        else if(sequence[0] == 'O') {
            // ESC O X
            switch (sequence[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            } 
        }

        return '\x1b';
    }
    else {
        return c;
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

/*** input ***/

void editor_move_cursor(int key) {
    switch (key) {
        case ARROW_UP: 
        {
            if(editorData.cy != 0) 
                editorData.cy--;
            break;
        } 
        case ARROW_DOWN: 
        {
            if(editorData.cy != editorData.screenrows - 1)
                editorData.cy++;
            break;
        }
        case ARROW_RIGHT: 
        {
            if(editorData.cx != editorData.screencols - 1)
                editorData.cx++;
            break;
        }
        case ARROW_LEFT: 
        {
            if(editorData.cx != 0)
                editorData.cx--;
            break;
        }
    }
}

// Function that deals with high-level input recieved from 
// editor_read_key() function that takes low-level job.
void editor_process_keypress() {
    int c = editor_read_key();
    
    switch (c) {
        case KEY_CTRL('q'):
        {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);            
            exit(0);
            break;
        }
        
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        {
            editor_move_cursor(c);
            break;
        }

        case PAGE_UP:
        {
            for (int i = 0; i < editorData.screenrows; i++) {
                editor_move_cursor(ARROW_UP);
            }
            break;
        }
        case PAGE_DOWN:
        {
            for (int i = 0; i < editorData.screencols; i++) {
                editor_move_cursor(ARROW_DOWN);
            }
            break;
        }

        case HOME_KEY:
        {
            editorData.cx = 0;
            break;
        }
        case END_KEY:
        {
            editorData.cx = editorData.screencols - 1;
            break;
        }
    }
}

/*** screen buffer ***/

typedef struct {
    char* buffer;
    int len;
} screenBuffer;

void screen_buffer_append(screenBuffer* sbuf, const char* str, int len) {
    char* new = realloc(sbuf->buffer, sbuf->len + len);

    if (new == NULL) {
        return; 
    }
    memcpy(&new[sbuf->len], str, len);
    sbuf->buffer = new;
    sbuf->len += len;
}

void screen_buffer_free(screenBuffer* sbuf) {
    free(sbuf->buffer);
}

/*** rendering ***/

void editor_draw_rows(screenBuffer* sbuf) {
    for (int y = 0; y < editorData.screenrows; y++) {
        screen_buffer_append(sbuf, "~", 1);

        screen_buffer_append(sbuf, "\x1b[K", 3);
        if (y < editorData.screenrows - 1) {
            screen_buffer_append(sbuf, "\r\n", 2);
        }
    } 
}

void editor_render_screen() {
    screenBuffer sbuf = {NULL, 0};
    
    screen_buffer_append(&sbuf, "\x1b[?25l", 6);
    screen_buffer_append(&sbuf, "\x1b[H", 3);
    
    editor_draw_rows(&sbuf);

    char cursor_move[32];
    snprintf(cursor_move, sizeof(cursor_move), "\x1b[%d;%dH", editorData.cy + 1, editorData.cx + 1);
    screen_buffer_append(&sbuf, cursor_move, strlen(cursor_move));
    screen_buffer_append(&sbuf, "\x1b[?25h", 6);

    write(STDOUT_FILENO, sbuf.buffer, sbuf.len);
}

/*** init ***/

void init_editor() {
    editorData.cx = 0;
    editorData.cy = 0;
    if (get_window_size(&editorData.screenrows, &editorData.screencols) == -1) 
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
