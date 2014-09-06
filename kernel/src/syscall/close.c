#include "syscalls/syscalls.h"

#include "bits/errno.h"

#include "process.h"

int sys_close(int fd) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  switch (current_process->fds[fd].type) {
  case PROCESS_FD_CLOSED:
    return -EBADF;
  case PROCESS_FD_TTY:
    current_process->fds[fd].u.tty = NULL;
    break;
  case PROCESS_FD_FILE:
    // TODO: keep a reference count and close the underlying file when it
    // reaches zero.
    current_process->fds[fd].u.file = NULL;
    break;
  }
  current_process->fds[fd].type = PROCESS_FD_CLOSED;
  return 0;
}
