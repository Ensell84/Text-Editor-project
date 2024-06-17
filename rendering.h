#ifndef RENDERING_H
#define RENDERING_H

typedef struct {
    char* buffer;
    int len;
} screenBuffer;

void screen_buffer_append(screenBuffer* sbuf, const char* str, int len);
void screen_buffer_free(screenBuffer* sbuf);
void editor_draw_rows(screenBuffer* sbuf);
void editor_render_screen();

#endif
