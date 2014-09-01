#include "log.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "portio.h"
#include "snprintf.h"

static void output(const char* str) {
  for (size_t i = 0; i < __builtin_strlen(str); i++) {
    outb(0xE9, str[i]);
  }
}

static void outputv(const char* format, va_list ap) {
  char s[80];
  size_t c = vsnprintf(s, sizeof(s), format, ap);
  if (c >= sizeof(s)) {
    c = sizeof(s);
    s[sizeof(s) - 4] = '.';
    s[sizeof(s) - 3] = '.';
    s[sizeof(s) - 2] = '.';
    s[sizeof(s) - 1] = '\0';
  }
  output(s);
}

static void outputf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  outputv(format, ap);
  va_end(ap);
}

void log(const char* w, const char* format, ...) {
  va_list ap;
  outputf("%-20s ", w);
  va_start(ap, format);
  outputv(format, ap);
  va_end(ap);
}

void panic(const char* w, const char* format, ...) {
  va_list ap;
  outputf("%-20s *** PANIC: ", w);
  va_start(ap, format);
  outputv(format, ap);
  va_end(ap);
  print_call_stack(0, (uint32_t)__builtin_frame_address(0));
  __asm__ __volatile__ ("cli; hlt");
  while (1) { }
}

void print_call_stack(uint32_t eip, uint32_t ebp) {
  const uintptr_t* pebp = (uintptr_t*)ebp;

  // Walk the stack frames.
  output("*** CALL STACK:");
  if (eip != 0) {
    outputf(" %08lx", eip);
  }
  while (1) {
    // Subtract 5 from the eip because that's the size of a call instruction.
    outputf(" %08lx", pebp[1] - 5);
    pebp = (uintptr_t*)pebp[0];
    if (pebp == 0 || pebp[1] == 0) {
      break;
    }
  }
  outputf("\n");
}
