#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"

pid_t sys_fork(struct isr_frame* frame) {
  struct process* child = NULL;
  int c = process_fork(&child, frame);
  if (c < 0) {
    // Error.
    return c;
  }
  // Always in the parent.  The child returns through a different path.
  assert(current_process != child);
  return child->pid;
}
