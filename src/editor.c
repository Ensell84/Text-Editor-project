#include "global_structs.h"
#include "editor.h"
#include "terminal.h"
#include <unistd.h>
#include <stdlib.h>

void editor_move_cursor(int key) {
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
            editorData.cx++;
            break;
        }
        case ARROW_LEFT: {
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
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);            
            exit(0);

        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editor_move_cursor(c);
            break;

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
    }
}
 
