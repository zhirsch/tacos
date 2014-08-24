#include "panic.h"

#include <stdarg.h>
#include <stdint.h>

void panic(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  kprintf("*** PANIC: ");
  kvprintf(format, ap);
  va_end(ap);

  {
    uintptr_t eip;
    uintptr_t* ebp;

    // Get the current eip.
    __asm__ __volatile__ ("call .L1\n"
                          ".L1: pop %%eax\n"
                          "mov %%eax, %0" : "=r" (eip) : : "eax");
    kprintf("*** PANIC: %08lx", eip);

    // Walk the stack frames.
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r" (ebp));
    while (1) {
      // Subtract 5 from the eip because that's the size of a call instruction.
      kprintf(" %08lx", ebp[1] - 5);
      ebp = (uintptr_t*)ebp[0];
      if (ebp == 0) {
        break;
      }
    }
    kprintf("\n");
  }

  __asm__ __volatile__ ("cli; hlt");
  while (1) { }
}
