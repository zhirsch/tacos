#ifndef SYSCALL_H
#define SYSCALL_H

#include "interrupts.h"

typedef void (*syscallfn)(struct isr_frame* frame);
extern syscallfn syscalls[];

void init_syscalls(void);

#endif /* SYSCALL_H */
