#include "syscalls/syscalls.h"

#include <stddef.h>

#include "bits/errno.h"
#include "bits/fcntl.h"

#include "log.h"
#include "process.h"
#include "tty.h"

#define PANIC(...) panic("SYSCALL [read]", __VA_ARGS__)

ssize_t sys_read(int fd, void* buf, size_t size, struct isr_frame* frame) {
  if (buf == NULL) {
    return -EFAULT;
  }
  if (current_process->fds[fd].type == PROCESS_FD_CLOSED) {
    return -EBADF;
  }
  if (current_process->fds[fd].mode & O_WRONLY) {
    return -EBADF;
  }
  if (current_process->fds[fd].type == PROCESS_FD_FILE) {
    PANIC("PROCESS_FD_FILE NOT IMPLEMENTED\n");
  }
  if (current_process->fds[fd].type == PROCESS_FD_TTY) {
    return tty_read(current_process->fds[fd].u.tty, buf, size);
  }
  return -EBADF;
}
