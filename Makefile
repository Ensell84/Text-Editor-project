text: main.c
	$(CC) main.c editor.c terminal.c rendering.c -o text -Wall -Wextra -pedantic -std=c99
