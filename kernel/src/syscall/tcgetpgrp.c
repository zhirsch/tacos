#include "syscalls/syscalls.h"

#include <stddef.h>

#include "bits/errno.h"

#include "log.h"
#include "process.h"
#include "tty.h"

#define PANIC(...) panic("SYSCALL [TCGETPGRP]", __VA_ARGS__)

pid_t sys_tcgetpgrp(int fd) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  if (current_process->fds[fd].file == NULL) {
    return -EBADF;
  }

  // TODO
#if 0
  if (current_process->fds[fd].file->tty == -1) {
    return -ENOTTY;
  }
  if (current_process->fds[fd].file->tty != current_process->tty) {
    return -ENOTTY;
  }
#endif
  if (ttys[current_process->tty].pgid == PGID_NONE) {
    PANIC("EDGE CASE: no pgid for tty\n");
  }

  return ttys[current_process->tty].pgid;
}
