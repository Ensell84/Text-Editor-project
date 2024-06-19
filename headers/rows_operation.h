#ifndef ROWS_OPERATION_H
#define ROWS_OPERATION_H

#include <sys/types.h>
#include <global_structs.h>

void editor_append_row(char* str, size_t len);
void editor_row_insert_symbol(Row* row, int at, int c); 
void editor_row_delete_symbol(Row* row, int at); 

#endif
