#include "rows_operation.h"
#include "global_structs.h"
#include <stdlib.h>
#include <string.h>

void editor_append_row(char* str, size_t len) {
    editorData.rows = realloc(editorData.rows, sizeof(Row) * (editorData.numrows + 1)); 
    
    int at = editorData.numrows;
    editorData.rows[at].row = malloc(sizeof(char) * (len + 1));
    editorData.rows[at].size = len;
    memcpy(editorData.rows[at].row, str, len);
    editorData.rows[at].row[len + 1] = '\0';
    editorData.numrows++;
}
