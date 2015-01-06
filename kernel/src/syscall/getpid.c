#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "process.h"

pid_t sys_getpid(struct isr_frame* frame) {
  return current_process->pid;
}
