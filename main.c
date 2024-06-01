#include <unistd.h>
#include <termios.h>

void enable_terminal_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    char c;
    enable_terminal_raw_mode();
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
        write(STDOUT_FILENO, &c, 1);        
    }
    return 0;
}
