#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"

int sys_sigsuspend(const sigset_t* mask, struct isr_frame* frame) {
  return -ENOSYS;
}
