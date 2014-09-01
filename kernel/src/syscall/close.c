#include <stdint.h>

#include "bits/errno.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [CLOSE]", __VA_ARGS__)

void syscall_close(struct isr_frame* frame) {
  syscall_in1(frame, int, fd, "%d");

  if (fd < 0 || fd > NUM_FDS) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  if (!current_process->fds[fd].used) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }

  current_process->fds[fd].used = 0;
  syscall_out(frame, 0, "%ld");
}
