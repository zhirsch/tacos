#ifndef TTY_H
#define TTY_H

#include "bits/types.h"

#define NUM_TTYS 1
#define TTY_NONE -1

struct tty {
  pid_t pgid;
};

extern struct tty ttys[NUM_TTYS];

void init_tty(void);

#endif /* TTY_H */
