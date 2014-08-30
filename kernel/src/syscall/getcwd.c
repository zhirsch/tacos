#include "syscall/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"

static const char CWD[] = "/";

void syscall_getcwd(struct isr_frame* frame) {
  if (frame->ecx < __builtin_strlen(CWD) + 1) {
    frame->eax = (uintptr_t)NULL;
    frame->ebx = 34;  // ERANGE
    return;
  }
  __builtin_memcpy((void*)frame->ebx, CWD, __builtin_strlen(CWD) + 1);
  frame->eax = frame->ebx;
}
