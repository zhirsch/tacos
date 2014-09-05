#include "syscalls/syscalls.h"

#include "bits/errno.h"

#include "process.h"

int sys_close(int fd) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  if (current_process->fds[fd].file == NULL) {
    return -EBADF;
  }

  // TODO
  /* current_process->fds[fd].used = 0; */
  return 0;
}
