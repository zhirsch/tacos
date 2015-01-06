#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "process.h"

gid_t sys_getegid(struct isr_frame* frame) {
  return current_process->egid;
}
