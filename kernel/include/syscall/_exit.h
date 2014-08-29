#ifndef SYSCALL__EXIT_H
#define SYSCALL__EXIT_H

#include "interrupts.h"

void syscall__exit(struct isr_frame* frame);

#endif /* SYSCALL__EXIT_H */
