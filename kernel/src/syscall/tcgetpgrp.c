#include "bits/errno.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"
#include "tty.h"

#define LOG(...) log("SYSCALL [TCGETPGRP]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [TCGETPGRP]", __VA_ARGS__)

void syscall_tcgetpgrp(struct isr_frame* frame) {
  syscall_in1(frame, int, fd, "%d");

  if (fd < 0 || fd > NUM_FDS) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  if (!current_process->fds[fd].used) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  if (current_process->fds[fd].tty == -1) {
    syscall_out(frame, -ENOTTY, "%ld");
    return;
  }
  if (current_process->fds[fd].tty != current_process->tty) {
    syscall_out(frame, -ENOTTY, "%ld");
    return;
  }
  if (ttys[current_process->tty].pgid == -1) {
    PANIC("EDGE CASE: no pgid for tty\n");
  }

  syscall_out(frame, ttys[current_process->tty].pgid, "%ld");
}