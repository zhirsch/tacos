#include "syscalls/syscalls.h"

#include "bits/types.h"

#include "interrupts.h"

pid_t sys_waitpid(pid_t pid, int* status, int options, struct isr_frame* frame) {
  // TODO
  return 0;
}
