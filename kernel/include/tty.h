#ifndef TTY_H
#define TTY_H

#include <stdint.h>

#include "bits/types.h"

#define NUM_TTYS 1
#define TTY_NONE -1
#define TTY_INPUT_RINGBUF_SIZE 128

struct tty {
  pid_t pgid;

  size_t    framebuf_pos;
  uint16_t* framebuf;

  char input_ringbuf[TTY_INPUT_RINGBUF_SIZE];
  size_t input_ringbuf_start;
  size_t input_ringbuf_end;
};

void init_tty(void);
struct tty* tty_get(int i);
void tty_setpgid(struct tty* tty, pid_t pgid);
int tty_read(struct tty* tty, void* buf, size_t count);
int tty_write(struct tty* tty, const void* buf, size_t count);

#endif /* TTY_H */
