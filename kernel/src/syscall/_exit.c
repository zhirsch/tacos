#include "syscall/syscalls.h"

#include "interrupts.h"
#include "panic.h"

void syscall__exit(struct isr_frame* frame) {
  panic("Process terminated with status %ld\n", frame->ebx);
}
