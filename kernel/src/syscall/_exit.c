#include "syscall/syscalls.h"

#include "interrupts.h"
#include "log.h"

#define PANIC(...) panic("SYSCALL[_EXIT]", __VA_ARGS__)

void syscall__exit(struct isr_frame* frame) {
  PANIC("Process terminated with status %ld\n", frame->ebx);
}
