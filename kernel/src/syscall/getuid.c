#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "process.h"

uid_t sys_getuid(struct isr_frame* frame) {
  return current_process->uid;
}
