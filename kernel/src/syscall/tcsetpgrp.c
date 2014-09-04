#include <stddef.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"
#include "tty.h"

#define LOG(...) log("SYSCALL [TCSETPGRP]", __VA_ARGS__)

void syscall_tcsetpgrp(struct isr_frame* frame) {
  syscall_in2(frame, int, fd, "%d", pid_t, pgrp, "%d");

  if (fd < 0 || fd > NUM_FDS) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  if (current_process->fds[fd].file == NULL) {
    syscall_out(frame, -EBADF, "%ld");
    return;
  }
  // TODO
  /* if (current_process->fds[fd].file->tty == -1) { */
  /*   syscall_out(frame, -ENOTTY, "%ld"); */
  /*   return; */
  /* } */
  /* if (current_process->fds[fd].file->tty != current_process->tty) { */
  /*   syscall_out(frame, -ENOTTY, "%ld"); */
  /*   return; */
  /* } */

  ttys[current_process->tty].pgid = pgrp;
  syscall_out(frame, 0, "%ld");
}
