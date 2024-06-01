#include <stdio.h>
#include <unistd.h>

// canonical mode --> raw mode
// read() ?
// STDIN_FILENO 

int main() {
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
       putc(c, stdout); 
    }
    return 0;
}
