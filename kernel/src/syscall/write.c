#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "screen.h"

void syscall_write(struct isr_frame* frame) {
  kprintf("WRITE: fd=%ld, buf=%08lx, count=%08lx\n", frame->ebx, frame->ecx, frame->edx);
  for (uint32_t i = 0; i < frame->edx; i++) {
    screen_writech(((const char*)frame->ecx)[i]);
  }
  frame->eax = frame->edx;
}
