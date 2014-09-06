#include "syscalls/syscalls.h"

#include <stddef.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "process.h"
#include "tty.h"

int sys_tcsetpgrp(int fd, pid_t pgrp) {
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
  // TODO: current_process->fds[fd].u.tty->pgid is not in the same session as current_process->pgrp -> ENOTTY
  // TODO: pgrp is not in the same session as current_process->pgrp -> EPERM
  current_process->tty->pgid = pgrp;
  return 0;
}
