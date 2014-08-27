#include "syscall/write.h"

#include <stdint.h>

#include "kprintf.h"
#include "screen.h"
#include "tss.h"

void syscall_write(struct tss* prev_tss) {
  // Switch to the process's address space.
  // TODO(zhirsch): This isn't safe for re-entry.
  __asm__ __volatile__ ("mov %0, %%cr3" : : "r" (prev_tss->cr3) : "memory");
  kprintf("WRITE: output %d characters to fd %d\n", prev_tss->edx, prev_tss->ebx);
  for (uint32_t i = 0; i < prev_tss->edx; i++) {
    putch(((const char*)prev_tss->ecx)[i]);
  }
  prev_tss->eax = prev_tss->edx;
}
