#include "syscalls/syscalls.h"

#include <stddef.h>

#include "bits/errno.h"

#include "interrupts.h"
#include "process.h"
#include "tty.h"

pid_t sys_tcgetpgrp(int fd, struct isr_frame* frame) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  if (current_process->fds[fd].type != PROCESS_FD_TTY) {
    return -EBADF;
  }
  if (current_process->tty == NULL) {
    return -ENOTTY;
  }
  if (current_process->fds[fd].u.tty != current_process->tty) {
    return -ENOTTY;
  }
  if (current_process->tty->pgid == PGID_NONE) {
    return process_next_pgid();
  }
  return current_process->tty->pgid;
}
