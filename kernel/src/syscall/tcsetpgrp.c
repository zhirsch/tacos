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
  if (current_process->fds[fd].file == NULL) {
    return -EBADF;
  }

  // TODO
#if 0
  if (current_process->fds[fd].file->tty == -1) {
    return -ENOTYY;
  }
  if (current_process->fds[fd].file->tty != current_process->tty) {
    return -ENOTYY;
  }
#endif

  ttys[current_process->tty].pgid = pgrp;
  return 0;
}
