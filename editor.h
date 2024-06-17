#ifndef EDITOR_H
#define EDITOR_H

#define KEY_CTRL(c) (c & 0x1f)
#include <termios.h>

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
} editorState;

extern editorState editorData;

void editor_process_keypress();
void editor_move_cursor(int key);

#endif

