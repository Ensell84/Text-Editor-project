#include "rows_operation.h"
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

void editor_row_insert_symbol(Row* row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;

    row->row = realloc(row->row, sizeof(char) * row->size + 2);
    memmove(&row->row[at + 1], &row->row[at], row->size - at + 1);
    row->row[at] = c;
    row->size++;
}

void editor_row_delete_symbol(Row* row, int at) {
    if (at < 0 || at > row->size) return;

    memmove(&row->row[at], &row->row[at + 1], row->size - at);
    row->size--;
}
