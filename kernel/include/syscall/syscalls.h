#ifndef SYSCALL_SYSCALLS_H
#define SYSCALL_SYSCALLS_H

#include "interrupts.h"

#define SYSCALL__EXIT  0x00
#define SYSCALL_WRITE  0x01
#define SYSCALL_SBRK   0x02
#define SYSCALL_GETCWD 0x03

#define NUM_SYSCALLS 4

const char* syscall_names[NUM_SYSCALLS];

void syscall__exit(struct isr_frame* frame);
void syscall_getcwd(struct isr_frame* frame);
void syscall_sbrk(struct isr_frame* frame);
void syscall_write(struct isr_frame* frame);

#endif /* SYSCALL_SYSCALLS_H */
