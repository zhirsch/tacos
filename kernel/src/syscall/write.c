#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "screen.h"

void syscall_write(struct isr_frame* frame) {
  kprintf("WRITE: output %ld characters to fd %ld\n", frame->edx, frame->ebx);
  for (uint32_t i = 0; i < frame->edx; i++) {
    screen_writech(((const char*)frame->ecx)[i]);
  }
  frame->eax = frame->edx;
}
