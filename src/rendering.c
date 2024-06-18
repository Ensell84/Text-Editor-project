#include "rendering.h"
#include "global_structs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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

void editor_scroll() {
    if (editorData.cy < editorData.rowoff) {
        editorData.rowoff = editorData.cy;
    }
    if (editorData.cy >= editorData.rowoff + editorData.screenrows) {
        editorData.rowoff = editorData.cy - editorData.screenrows + 1;
    }
    if (editorData.cx < editorData.coloff) {
        editorData.coloff = editorData.cx;
    }
    if (editorData.cx >= editorData.coloff + editorData.screencols) {
        editorData.coloff = editorData.cx - editorData.screencols + 1;
    }
}

void editor_draw_rows(screenBuffer* sbuf) {
    for (int y = 0; y < editorData.screenrows; y++) {
        int filerow = y + editorData.rowoff;

        if (filerow >= editorData.numrows) {
            screen_buffer_append(sbuf, "~", 1);
        }
        else {
            int len = editorData.rows[filerow].size - editorData.coloff;
            if(len < 0) len = 0;
            if (len > editorData.screencols) len = editorData.screencols;
            screen_buffer_append(sbuf, &editorData.rows[filerow].row[editorData.coloff], len);
        }

        screen_buffer_append(sbuf, "\x1b[K", 3);
        if (y < editorData.screenrows - 1) {
            screen_buffer_append(sbuf, "\r\n", 2);
        }
    } 
}

void editor_render_screen() {
    editor_scroll();
    screenBuffer sbuf = {NULL, 0};

    screen_buffer_append(&sbuf, "\x1b[?25l", 6);
    screen_buffer_append(&sbuf, "\x1b[H", 3);

    editor_draw_rows(&sbuf);

    char cursor_move[32];
    snprintf(cursor_move, sizeof(cursor_move), "\x1b[%d;%dH", (editorData.cy - editorData.rowoff) + 1, (editorData.cx - editorData.coloff) + 1);
    screen_buffer_append(&sbuf, cursor_move, strlen(cursor_move));

    screen_buffer_append(&sbuf, "\x1b[?25h", 6);

    write(STDOUT_FILENO, sbuf.buffer, sbuf.len);
    screen_buffer_free(&sbuf);
}

