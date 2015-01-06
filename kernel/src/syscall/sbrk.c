#include "syscalls/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "mmu/sbrk.h"
#include "process.h"

void* sys_sbrk(intptr_t increment, struct isr_frame* frame) {
  void* vaddr = (void*)current_process->program_break;
  if (increment < 0) {
    mmu_sbrk_shrink(&current_process->program_break, -increment);
  } else if (increment > 0) {
    mmu_sbrk_grow(&current_process->program_break, increment);
  }
  return vaddr;
}
