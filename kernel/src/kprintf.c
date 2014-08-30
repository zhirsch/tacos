#include "kprintf.h"

#include <stdarg.h>
#include <stddef.h>

#include "portio.h"
#include "snprintf.h"

void kprintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  kvprintf(format, ap);
  va_end(ap);
}

void kvprintf(const char* format, va_list ap) {
  char s[80];
  size_t c;
  c = vsnprintf(s, sizeof(s), format, ap);
  if (c >= sizeof(s)) {
    c = sizeof(s);
    s[sizeof(s) - 4] = '.';
    s[sizeof(s) - 3] = '.';
    s[sizeof(s) - 2] = '.';
    s[sizeof(s) - 1] = '\n';
  }
  for (size_t i = 0; i < c; i++) {
    outb(0xe9, s[i]);
  }
}
