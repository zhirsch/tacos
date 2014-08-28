#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

#include "kprintf.h"

void panic(const char* format, ...) __attribute__ ((noreturn, format(printf, 1, 2)));
void print_call_stack(uint32_t eip, uint32_t ebp);

#endif /* PANIC_H */
