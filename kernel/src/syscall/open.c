#include "syscall/syscalls.h"

#include <stdint.h>

#include "bits/types.h"

#include "interrupts.h"
#include "kprintf.h"

void syscall_open(struct isr_frame* frame) {
  const char* pathname = (const char*)frame->ebx;
  const int flags = frame->ecx;
  const mode_t mode = frame->edx;
  kprintf("OPEN: pathname=%s, flags=%08x, mode=%08lx\n", pathname, flags, mode);
  frame->eax = -2;  // ENOENT
}
