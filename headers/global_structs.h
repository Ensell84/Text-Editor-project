#ifndef GLOBAL_STRUCTS_H
#define GLOBAL_STRUCTS_H

#include <termios.h>

enum editorKey {
    BACKSPACE = 127,
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

typedef struct Row {
    char* row;
    int size;
} Row;

typedef struct {
    int cx, cy;
    int screenrows;
    int screencols;
    int rowoff;
    int coloff;
    int numrows;
    Row* rows;
    struct termios original_termios;
    char* filename;
} editorState;

extern editorState editorData;

#endif
