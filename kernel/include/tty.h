#ifndef TTY_H
#define TTY_H

#include "bits/types.h"

struct tty {
  pid_t pgid;
};

#define NUM_TTYS 1

extern struct tty ttys[NUM_TTYS];

void init_tty(void);

#endif /* TTY_H */
