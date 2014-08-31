#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "process.h"

void syscall_close(struct isr_frame* frame) {
  const uint32_t fd = frame->ebx;
  kprintf("CLOSE: fd=%ld\n", fd);

  if (fd > NUM_FDS) {
    kprintf("CLOSE: Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    kprintf("CLOSE: Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  current_process->fds[fd].used = 0;
  frame->eax = 0;
}
