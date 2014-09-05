#include "syscalls/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"
#include "bits/fcntl.h"

#include "process.h"

static int dupfd(int oldfd, int newfd);
static int setfd(int fd, int val);

int sys_fcntl(int fd, int cmd, uint32_t arg) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  if (current_process->fds[fd].type == PROCESS_FD_CLOSED) {
    return -EBADF;
  }

  switch (cmd) {
  case F_DUPFD:
    return dupfd(fd, arg);
  case F_SETFD:
    return setfd(fd, arg);
  default:
    return -ERANGE;
  }
}

static int dupfd(int oldfd, int newfd) {
  for (int i = newfd; i < NUM_FDS; i++) {
    if (current_process->fds[i].type == PROCESS_FD_CLOSED) {
      current_process->fds[i] = current_process->fds[oldfd];
      return i;
    }
  }
  return -EMFILE;
}

static int setfd(int fd, int val) {
  switch (val) {
  case FD_CLOEXEC:
    current_process->fds[fd].mode |= FD_CLOEXEC;
    return 0;
  default:
    return -ERANGE;
  }
}
