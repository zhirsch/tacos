#ifndef SYSCALL_INIT_H
#define SYSCALL_INIT_H

enum syscalls {
  SYSCALL__EXIT = 0x00,
  SYSCALL_WRITE = 0x01,
};

#define NUM_SYSCALLS  2

const char* syscall_names[NUM_SYSCALLS];

void init_syscalls(void);

#endif /* SYSCALL_INIT_H */
