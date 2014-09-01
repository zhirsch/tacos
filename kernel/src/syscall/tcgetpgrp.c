#include "syscall/syscalls.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "tty.h"

#define LOG(...) log("SYSCALL[TCGETPGRP]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL[TCGETPGRP]", __VA_ARGS__)

void syscall_tcgetpgrp(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  LOG("fd=%d\n", fd);

  if (fd < 0 || fd > NUM_FDS) {
    LOG("Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    LOG("Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (current_process->fds[fd].tty == -1) {
    LOG("No tty for fd\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  if (current_process->fds[fd].tty != current_process->tty) {
    LOG("Not the controlling terminal\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  if (ttys[current_process->tty].pgid == -1) {
    PANIC("EDGE CASE: no pgid for tty\n");
  }

  LOG("tty=%d, pgid=%d\n", current_process->tty, ttys[current_process->tty].pgid);
  frame->eax = ttys[current_process->tty].pgid;
}
