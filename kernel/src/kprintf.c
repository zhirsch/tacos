#include "kprintf.h"

#include <stdarg.h>

#include "screen.h"
#include "snprintf.h"

void kprintf(const char* format, ...) {
  va_list ap;
  char s[512];
  int c;

  va_start(ap, format);
  c = vsnprintf(s, sizeof(s) - 1, format, ap);
  va_end(ap);

  s[c] = '\0';
  puts(s);
}
