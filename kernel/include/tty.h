#ifndef TTY_H
#define TTY_H

#include <stdint.h>

#include "bits/types.h"

#define NUM_TTYS 1

struct tty {
  pid_t pgid;

  size_t    framebuf_pos;
  uint16_t* framebuf;

  size_t inputbuf_pos;
  uint16_t* inputbuf;
};

void init_tty(void);
struct tty* tty_get(int i);
void tty_setpgid(struct tty* tty, pid_t pgid);
int tty_read(struct tty* tty, void* buf, size_t count);
int tty_write(struct tty* tty, const void* buf, size_t count);

void tty_putch(char ch);

#endif /* TTY_H */
