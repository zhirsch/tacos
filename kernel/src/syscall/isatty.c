#include "syscall/syscalls.h"

#include "interrupts.h"

void syscall_isatty(struct isr_frame* frame) {
  // TODO(zhirsch): Better implementation...
  const int fd = frame->ebx;
  frame->eax = (fd == 0 || fd == 1 || fd == 2);
}
