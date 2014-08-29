#ifndef SYSCALL_SYSCALLS_H
#define SYSCALL_SYSCALLS_H

#include "interrupts.h"

enum syscalls {
  SYSCALL__EXIT = 0x00,
  SYSCALL_WRITE = 0x01,
  SYSCALL_SBRK  = 0x02,
};

#define NUM_SYSCALLS 3

const char* syscall_names[NUM_SYSCALLS];

void syscall__exit(struct isr_frame* frame);
void syscall_write(struct isr_frame* frame);
void syscall_sbrk(struct isr_frame* frame);

#endif /* SYSCALL_SYSCALLS_H */
