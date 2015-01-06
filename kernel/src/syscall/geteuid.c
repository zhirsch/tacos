#include "syscalls/syscalls.h"

#include "process.h"

#include "interrupts.h"

uid_t sys_geteuid(struct isr_frame* frame) {
  return current_process->euid;
}
