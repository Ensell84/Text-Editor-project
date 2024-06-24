SRC = src/main.c src/editor.c src/terminal.c src/rendering.c src/rows_operation.c

text: src/main.c
	$(CC) $(SRC) -O3 -o text -Iheaders -Wall -Wextra -pedantic -std=c99
