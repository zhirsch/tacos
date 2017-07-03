#include "syscalls/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"
#include "bits/fcntl.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "screen.h"
#include "tty.h"

#define PANIC(...) panic("SYSCALL [WRITE]", __VA_ARGS__)

ssize_t sys_write(int fd, const void* buf, size_t count, struct isr_frame* frame) {
  if (buf == NULL) {
    return -EFAULT;
  }
  if (current_process->fds[fd].type == PROCESS_FD_CLOSED) {
    return -EBADF;
  }
  if (current_process->fds[fd].mode & O_RDONLY) {
    return -EBADF;
  }
  if (current_process->fds[fd].type == PROCESS_FD_FILE) {
    PANIC("PROCESS_FD_FILE NOT IMPLEMENTED\n");
  }
  if (current_process->fds[fd].type == PROCESS_FD_TTY) {
    return tty_write(current_process->fds[fd].u.tty, buf, count);
  }
  return -EBADF;
}
