#include "global_structs.h"
#include "editor.h"
#include "rows_operation.h"
#include "terminal.h"
#include <unistd.h>
#include <stdlib.h>

void editor_move_cursor(int key) {
    Row* row = (editorData.cy >= editorData.numrows) ? NULL : &editorData.rows[editorData.cy];

    switch (key) {
        case ARROW_UP: {
            if(editorData.cy != 0) 
                editorData.cy--;
            break;
        }
        case ARROW_DOWN: {
            if(editorData.cy < editorData.numrows)
                editorData.cy++;
            break;
        }
        case ARROW_RIGHT: {
            if (row && editorData.cx < row->size) {
                editorData.cx++;
            }
            break;
        }
        case ARROW_LEFT: {
            if(editorData.cx != 0)
                editorData.cx--;
            break;
        }
    }

    row = (editorData.cy >= editorData.numrows) ? NULL : &editorData.rows[editorData.cy];
    int rowlength = row ? row->size : 0;
    if (editorData.cx > rowlength) {
        editorData.cx = rowlength;
    }
}

void editor_insert_char (int c) {
    if (editorData.cy == editorData.numrows) {
        editor_append_row("", 0);
    }
    editor_row_insert_symbol(&editorData.rows[editorData.cy], editorData.cx, c);
    editorData.cx++;
}

void editor_delete_char () {
    if (editorData.cy == editorData.numrows) {
        return;
    }
    if (editorData.cx > 0) {
        editor_row_delete_symbol(&editorData.rows[editorData.cy], editorData.cx - 1);
        editorData.cx--;
    }
}

// Function that deals with high-level input recieved from
// editor_read_key() function that takes low-level job.
void editor_process_keypress() {
    int c = editor_read_key();
    
    switch (c) {
        case '\r': {
            // TODO:
            break;
        }

        case BACKSPACE:
        case KEY_CTRL('h'):
        case DELETE_KEY: {
            if (c == DELETE_KEY) editor_move_cursor(ARROW_RIGHT);
            editor_delete_char();
            break;
        }

        case KEY_CTRL('q'): {
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);            
            exit(0);
        }
        case KEY_CTRL('s'): {
            editor_save();
            break;
        }

        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT: {
            editor_move_cursor(c);
            break;
        }

        case PAGE_UP: {
            for (int i = 0; i < editorData.screenrows; i++) {
                editor_move_cursor(ARROW_UP);
            }
            break;
        }
        case PAGE_DOWN: {
            for (int i = 0; i < editorData.screencols; i++) {
                editor_move_cursor(ARROW_DOWN);
            }
            break;
        }

        case HOME_KEY: {
            editorData.cx = 0;
            break;
        }
        case END_KEY: {
            editorData.cx = editorData.screencols - 1;
            break;
        }
        
        // ignoring ESC press and CTRL+L
        case KEY_CTRL('l'):
        case '\x1b': {
            break;
        }

        default: {
            editor_insert_char(c);
            break;
        }
    }
} 
