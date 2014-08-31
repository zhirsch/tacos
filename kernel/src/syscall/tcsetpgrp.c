#include "syscall/syscalls.h"

#include "bits/types.h"

#include "interrupts.h"
#include "kprintf.h"
#include "panic.h"
#include "process.h"
#include "tty.h"

void syscall_tcsetpgrp(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  const pid_t pgrp = (pid_t)frame->ecx;
  kprintf("TCSETPGRP: fd=%d, pgrp=%d\n", fd, pgrp);

  if (fd < 0 || fd > NUM_FDS) {
    kprintf("TCSETPGRP: Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    kprintf("TCSETPGRP: Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (current_process->fds[fd].tty == -1) {
    kprintf("TCSETPGRP: No tty for fd\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  if (current_process->fds[fd].tty != current_process->tty) {
    kprintf("TCSETPGRP: Not the controlling terminal\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  ttys[current_process->tty].pgid = pgrp;
  frame->eax = 0;
}
