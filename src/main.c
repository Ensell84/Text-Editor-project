#include <stdio.h>
#include "global_structs.h"
#include "editor.h"
#include "terminal.h"
#include "rendering.h"

editorState editorData;

void init_editor() {
    editorData.filename = NULL;
    editorData.cx = 0;
    editorData.cy = 0;
    editorData.rowoff = 0;
    editorData.coloff = 0;
    editorData.rows = NULL;
    editorData.numrows = 0;
    if (get_window_size(&editorData.screenrows, &editorData.screencols) == -1) 
        error_handler("get_window_size() in init_editor()");
}

int main(int argc, char** argv) {
    enable_terminal_raw_mode();
    init_editor();
    if (argc >= 2) {
        editor_open(argv[1]);
        editorData.filename = argv[1];
    }
    
    editor_render_screen();
    for(;;) {
        editor_process_keypress();
        editor_render_screen();
    }
     
    return 0;
}
 
