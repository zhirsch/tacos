#ifndef SYSCALL_WRITE_H
#define SYSCALL_WRITE_H

#include "interrupts.h"

void syscall_write(struct isr_frame* frame);

#endif /* SYSCALL_WRITE_H */
