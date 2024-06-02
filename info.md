

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
tcgetattr(int fd, struct termios *termios_p)
tcsetattr(int fd, int optional_actions, const struct termios *termios_p)
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


### miscellaneous functions

`atexit(func)` ---function from <stdlib.h> which executes func at exit() call or 
when program returns from main function.

`TCSAFLUSH` argument specifies when to apply the change: it waits for all 
pending output to be written to the terminal, and also discards any input
that hasn’t been read.


