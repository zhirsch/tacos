#include "syscall/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "process.h"

void syscall_getcwd(struct isr_frame* frame) {
  char* const buf = (char*)frame->ebx;
  const size_t size = frame->ecx;
  if (size < __builtin_strlen(current_process->cwd) + 1) {
    frame->eax = -34;  // ERANGE
    return;
  }
  __builtin_strncpy(buf, current_process->cwd, size);
  frame->eax = frame->ebx;
}
