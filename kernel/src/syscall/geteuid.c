#include "syscall/syscalls.h"

#include "interrupts.h"
#include "process.h"

void syscall_geteuid(struct isr_frame* frame) {
  frame->eax = current_process->euid;
}
