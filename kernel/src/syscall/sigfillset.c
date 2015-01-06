#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/signal.h"

#include "interrupts.h"

int sys_sigfillset(sigset_t* set, struct isr_frame* frame) {
  if (set == NULL) {
    return -EFAULT;
  }

  *set = (sigset_t)-1;
  return 0;
}
