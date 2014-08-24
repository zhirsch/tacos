#include "kprintf.h"

#include <stdarg.h>

#include "screen.h"
#include "snprintf.h"

void kprintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  kvprintf(format, ap);
  va_end(ap);
}

void kvprintf(const char* format, va_list ap) {
  char s[512];
  int c;
  c = vsnprintf(s, sizeof(s) - 1, format, ap);
  s[c] = '\0';
  puts(s);
}
