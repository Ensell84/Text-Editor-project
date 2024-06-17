#include "rendering.h"
#include "editor.h"
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

void editor_draw_rows(screenBuffer* sbuf) {
    for (int y = 0; y < editorData.screenrows; y++) {
        screen_buffer_append(sbuf, "~", 1);
        // ...
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
    screen_buffer_free(&sbuf);
}

