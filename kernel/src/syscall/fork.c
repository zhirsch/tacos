#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"
#include "process.h"

pid_t sys_fork(struct isr_frame* frame) {
  struct process* child = NULL;
  int c = process_fork(&child, frame);
  if (c < 0) {
    // Error.
    return c;
  }
  if (current_process == child) {
    // In the child.
    return 0;
  }
  // In the parent.
  return child->pid;
}
