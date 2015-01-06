#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"

int sys_sigprocmask(int how, const sigset_t* set, sigset_t* oldset, struct isr_frame* frame) {
  return -ENOSYS;
}
