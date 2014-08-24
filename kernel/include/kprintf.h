#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>

void kprintf(const char* format, ...) __attribute__ ((format(printf, 1, 2)));
void kvprintf(const char* format, va_list ap);

#endif /* KPRINTF_H */
