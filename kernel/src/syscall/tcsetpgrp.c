#include "syscall/syscalls.h"

#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "tty.h"

#define LOG(...) log("SYSCALL[TCSETPGRP]", __VA_ARGS__)

void syscall_tcsetpgrp(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  const pid_t pgrp = (pid_t)frame->ecx;
  LOG("fd=%d, pgrp=%d\n", fd, pgrp);

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
  ttys[current_process->tty].pgid = pgrp;
  frame->eax = 0;
}
