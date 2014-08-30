#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "mmu.h"
#include "panic.h"
#include "sbrk.h"

void syscall_sbrk(struct isr_frame* frame) {
  const intptr_t increment = frame->ebx;
  const uintptr_t vaddr = current_program_break;
  kprintf("SBRK: Increase program break by %08lx\n", increment);

  if (increment < 0) {
    sbrk_shrink(&current_program_break, -increment);
  } else if (increment > 0) {
    sbrk_grow(&current_program_break, increment);
  }
  kprintf("SBRK: Returning %08lx, new program break %08lx\n", vaddr, current_program_break);
  frame->eax = vaddr;
}
