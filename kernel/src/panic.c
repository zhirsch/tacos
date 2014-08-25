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
    const uintptr_t* ebp = (uintptr_t*)__builtin_frame_address(0);

    // Walk the stack frames.
    kprintf("*** CALL STACK:");
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
