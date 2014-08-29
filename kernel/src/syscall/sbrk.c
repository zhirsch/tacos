#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "mmu.h"

void syscall_sbrk(struct isr_frame* frame) {
  const intptr_t increment = frame->ebx;
  const uintptr_t vaddr = current_program_break;
  kprintf("SBRK: Increase program break by %08lx\n", increment);
  /* // Make sure that a multiple of a page was requested. */
  /* if (increment & (PAGESIZE - 1)) { */
  /*   kprintf("SBRK: Increment is not page aligned: %08lx\n", increment); */
  /*   frame->eax = -1; */
  /*   frame->ebx = 22;  // EINVAL */
  /*   return; */
  /* } */

  if (increment < 0) {
    // Shrink the heap.
    for (intptr_t i = 0; i < -increment; i += PAGESIZE) {
      void* paddr;
      current_program_break -= PAGESIZE;
      paddr = mmu_unmap_page((void*)current_program_break);
      mmu_release_physical_page(paddr);
    }
  } else if (increment > 0) {
    // Grow the heap.
    for (intptr_t i = 0; i < increment; i += PAGESIZE) {
      void* paddr;
      paddr = mmu_acquire_physical_page();
      mmu_map_page(paddr, (void*)current_program_break, 0x1 | 0x2 | 0x4);
      current_program_break += PAGESIZE;
    }
  }
  kprintf("SBRK: Returning %08lx, new program break %08lx\n", vaddr, current_program_break);
  frame->eax = vaddr;
}
