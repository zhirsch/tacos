#include "syscall/write.h"

#include "kprintf.h"
#include "tss.h"

void syscall_write(struct tss* prev_tss) {
  // Switch to the process's address space.
  // TODO(zhirsch): This isn't safe for re-entry.
  __asm__ __volatile__ ("mov %0, %%cr3" : : "r" (prev_tss->cr3) : "memory");
  kprintf("%.*s", prev_tss->edx, (const char*)prev_tss->ecx);
  prev_tss->eax = prev_tss->edx;
}
