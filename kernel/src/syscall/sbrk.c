#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "sbrk.h"

#define LOG(...) log("SYSCALL[SBRK]", __VA_ARGS__)

void syscall_sbrk(struct isr_frame* frame) {
  const intptr_t increment = frame->ebx;
  const uintptr_t vaddr = current_process->program_break;
  LOG("Increase program break by %08lx\n", increment);

  if (increment < 0) {
    sbrk_shrink(&current_process->program_break, -increment);
  } else if (increment > 0) {
    sbrk_grow(&current_process->program_break, increment);
  }
  LOG("Returning %08lx, new program break %08lx\n", vaddr, current_process->program_break);
  frame->eax = vaddr;
}
