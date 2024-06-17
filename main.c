#include "editor.h"
#include "terminal.h"
#include "rendering.h"

editorState editorData;

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
 
