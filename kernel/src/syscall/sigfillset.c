#include "syscall/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "bits/signal.h"

#include "interrupts.h"
#include "kprintf.h"

void syscall_sigfillset(struct isr_frame* frame) {
  sigset_t* set = (sigset_t*)frame->ebx;
  if (set == NULL) {
    frame->eax = -14;  // EFAULT
    return;
  }
  __builtin_memset(set, 0xff, sizeof(*set));
  frame->eax = 0;
}
