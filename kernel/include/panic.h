#ifndef PANIC_H
#define PANIC_H

#include "kprintf.h"

static inline void clihlt(void) __attribute__ ((noreturn));
static inline void clihlt(void) {
  __asm__ __volatile__ ("cli; hlt");
  while (1) { }
}

#define panic(fmt, ...) { kprintf(fmt, __VA_ARGS__); clihlt(); }
#define panic0(msg) panic("%s", msg)

#endif /* PANIC_H */
