#ifndef EDITOR_H
#define EDITOR_H

#define KEY_CTRL(c) (c & 0x1f)

void editor_process_keypress();
void editor_move_cursor(int key);

#endif

