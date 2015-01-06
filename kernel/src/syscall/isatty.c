#include "syscalls/syscalls.h"

#include "bits/errno.h"

#include "interrupts.h"
#include "process.h"

int sys_isatty(int fd, struct isr_frame* frame) {
  if (fd < 0 || fd > NUM_FDS) {
    return -EBADF;
  }
  return (current_process->fds[fd].type == PROCESS_FD_TTY);
}
