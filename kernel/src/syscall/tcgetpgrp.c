#include "syscall/syscalls.h"

#include "interrupts.h"
#include "kprintf.h"
#include "panic.h"
#include "process.h"
#include "tty.h"

void syscall_tcgetpgrp(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  kprintf("TCGETPGRP: fd=%d\n", fd);

  if (fd < 0 || fd > NUM_FDS) {
    kprintf("TCGETPGRP: Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    kprintf("TCGETPGRP: Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (current_process->fds[fd].tty == -1) {
    kprintf("TCGETPGRP: No tty for fd\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  if (current_process->fds[fd].tty != current_process->tty) {
    kprintf("TCGETPGRP: Not the controlling terminal\n");
    frame->eax = -25;  // ENOTTY
    return;
  }
  if (ttys[current_process->tty].pgid == -1) {
    panic("TCGETPGRP: EDGE CASE: no pgid for tty\n");
  }

  kprintf("TCGETPGRP: tty=%d, pgid=%d\n", current_process->tty,
          ttys[current_process->tty].pgid);
  frame->eax = ttys[current_process->tty].pgid;
}
