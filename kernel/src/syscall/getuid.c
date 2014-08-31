#include "syscall/syscalls.h"

#include "interrupts.h"
#include "process.h"

void syscall_getuid(struct isr_frame* frame) {
  frame->eax = current_process->uid;
}
