#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "process.h"

gid_t sys_getgid(struct isr_frame* frame) {
  return current_process->gid;
}
