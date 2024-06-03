

### read() and file descriptors
A file descriptor is an integer that uniquely identifies an open file within a process

### terminal modes(raw and canonical) + termios.h

termios structure:
```C
struct termios {
	tcflag_t        c_iflag;        /* input flags */
	tcflag_t        c_oflag;        /* output flags */
	tcflag_t        c_cflag;        /* control flags */
	tcflag_t        c_lflag;        /* local flags */
	cc_t            c_cc[NCCS];     /* control chars */
	speed_t         c_ispeed;       /* input speed */
	speed_t         c_ospeed;       /* output speed */
};
```

`tcflag_t` is just unsigned long:
```C
typedef unsigned long   tcflag_t;
```

ECHO flag in <termios.h>:
```C
#define ECHO            0x00000008      /* enable echoing */
```
so, ECHO is just bitflag - 00000000000000000000000000001000.

`raw.c_lflag &= ~(ECHO);` -> to set flag:
- `~` bitwise NOT operator -> 11111111111111111111111111110111
- `&` bitwise AND operator -> sets our flag to 0


Functions to manipulate termios structure associated with terminal:
```C
tcgetattr(int fd, struct termios *termios_p);
tcsetattr(int fd, int optional_actions, const struct termios *termios_p);
```

* first parameter of functions -> file descriptor
  It is used, because in unix-like systems terminals are treated as files.
* optional_actions --> when the changes should take effect (`TCSANOW`, `TCSADRAIN`, or `TCSAFLUSH`)

file descritor is needed because we can open new termanal in our program and
modify its termios structure using its file descriptor:
```C
int term_fd = open("/dev/tty1", O_RDWR);
```
by passing `STDIN_FILENO` we work with terminal from wich our program is
started(controlling terminal for that session)

### Other termios flags:
- `ICANON` -- disables canonical mode now every keypress sends directly to program
- `ISIG` -- disables Ctrl + Z / Ctrl + C / Ctrl + Y
- `IXON` -- disable Ctrl + S / Ctrl + Q
- `IEXTEN` -- disable Ctrl + V delay
- `ICRNL` -- disables mapping Ctrl + M(CR) to ENTER
- `OPOST` -- turn off output processing
	- by default terminal translates '\n' -> '\r\n' 
	- when disabled -> after pressing ENT cursor only moves down

Special control characters (`c_cc` field of termios structure):
- `VMIN` -- minimum amount of bytes to `read()` return
- `VTIME` -- amount of time `read()` wates before return
if `read()` times out -> it returns 0

### miscellaneous functions

`atexit(func)` --- function from <stdlib.h> which executes func at exit() call or 
when program returns from main function.

`TCSAFLUSH` argument specifies when to apply the change: it waits for all 
pending output to be written to the terminal, and also discards any input
that hasn’t been read.


### Asci Symbols
0 - 31 + 127 --- Control Characters
32 - 126 --- Printable characters

Arrow keys + pageUp/pageDown + home/end --- send 3 or 4 bytes to term
- 27[C -- Right Arrow
- 27[D -- Left Arrow
- ...

They all start from ESC(27) and called **Escape Sequences**

Other Symbols and ESC sequences
- Enter - 10 ('\n')
- Backscape - 127 
- Ctrl + A - 1
- Ctrl + B - 2
- ...
- Ctrl + Z - 26

To be aware of:
- Ctrl + C -- terminates program (SIGINT)
- Ctrl + Y/Z -- suspends program (SIGTSTP)
- Ctrl + S -- program stops sending you output (until u press Ctrl + Q)
    - it is named software flow control (link...)
- Ctrl + V -- waits for next symbol to send it to terminal 
- Ctrl + M -- translates to ENTER(10), because terminal translates 13(Ctrl+M) which is CR('\r') to new line - ENTER('\n')

`iscntrl()` --- function from <ctype.h>, detects cntrl symbols

- как отличить TAB от Ctrl + A если оба map-аются на 9 

### Error handling
Most C stand func when fail set global var `errno` to indicate that error happened
    and also most times they return `-1` value
`perror(str)` --- looks at `errno` and prints descriptive error msg + str given to it

