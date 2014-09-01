#include <stdint.h>

#include "bits/errno.h"
#include "bits/fcntl.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "string.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [FCNTL]", __VA_ARGS__)

static int dupfd(int oldfd, int newfd);
static int setfd(int fd, int val);

void syscall_fcntl(struct isr_frame* frame) {
  syscall_in3(frame, int, fd, "%d", int, cmd, "%d", uint32_t, arg, "%08lx");

  if (fd < 0 || fd > NUM_FDS) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  if (!current_process->fds[fd].used) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }

  switch (cmd) {
  case F_DUPFD:
    syscall_out(frame, dupfd(fd, arg), "%ld");
    return;
  case F_SETFD:
    syscall_out(frame, setfd(fd, arg), "%ld");
    return;
  default:
    syscall_out(frame, -ERANGE, "%ld");
    return;
  }
}

static int dupfd(int oldfd, int newfd) {
  for (int i = newfd; i < NUM_FDS; i++) {
    if (!current_process->fds[i].used) {
      current_process->fds[i] = current_process->fds[oldfd];
      return i;
    }
  }
  return -EMFILE;
}

static int setfd(int fd, int val) {
  switch (val) {
  case FD_CLOEXEC:
    current_process->fds[fd].cloexec = 1;
    return 0;
  default:
    return -ERANGE;
  }
}
