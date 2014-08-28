#include "panic.h"

#include <stdarg.h>
#include <stdint.h>

void panic(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  kprintf("*** PANIC: ");
  kvprintf(format, ap);
  va_end(ap);
  print_call_stack(0, (uint32_t)__builtin_frame_address(0));
  __asm__ __volatile__ ("cli; hlt");
  while (1) { }
}

void print_call_stack(uint32_t eip, uint32_t ebp) {
  const uintptr_t* pebp = (uintptr_t*)ebp;

  // Walk the stack frames.
  kprintf("*** CALL STACK:");
  if (eip != 0) {
    kprintf(" %08lx", eip);
  }
  while (1) {
    // Subtract 5 from the eip because that's the size of a call instruction.
    kprintf(" %08lx", pebp[1] - 5);
    pebp = (uintptr_t*)pebp[0];
    if (pebp == 0 || pebp[1] == 0) {
      break;
    }
  }
  kprintf("\n");
}
