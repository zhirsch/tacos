#ifndef PANIC_H
#define PANIC_H

#include "kprintf.h"

void panic(const char* format, ...) __attribute__ ((noreturn));

#endif /* PANIC_H */
