#include "syscall/syscalls.h"

#include <stdint.h>

#include "bits/types.h"

#include "interrupts.h"
#include "kprintf.h"
#include "panic.h"
#include "screen.h"

void syscall_waitpid(struct isr_frame* frame) {
  const pid_t pid = (pid_t)frame->ebx;
  int* const status = (int*)frame->ecx;
  const int options = (int)frame->edx;
  kprintf("WAITPID: pid=%d, status=%08lx, options=%08x\n", pid, (uintptr_t)status, options);
  frame->eax = 0;  // TODO
}
