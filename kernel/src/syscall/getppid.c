#include "syscalls/syscalls.h"

#include "process.h"

#include "interrupts.h"

pid_t sys_getppid(struct isr_frame* frame) {
  return current_process->ppid;
}
