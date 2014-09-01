#ifndef PANIC_H
#define PANIC_H

#include <stdint.h>

void log(const char* w, const char* format, ...) __attribute__ ((format(printf, 2, 3)));
void panic(const char* w, const char* format, ...) __attribute__ ((noreturn, format(printf, 2, 3)));

void print_call_stack(uint32_t eip, uint32_t ebp);

#endif /* PANIC_H */
