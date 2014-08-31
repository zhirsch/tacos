#include "syscall/init.h"

#include <stddef.h>

#include "interrupts.h"
#include "panic.h"
#include "syscall/syscalls.h"

struct {
  const char* name;
  void (*func)(struct isr_frame* frame);
} syscalls[42];

#define define_syscall(n)                                               \
  do {                                                                  \
    syscalls[SYSCALL_##n].name = #n;                                    \
    syscalls[SYSCALL_##n].func = syscall_##n;                           \
  } while (0)


static void syscall_handler(struct isr_frame* frame);

void init_syscalls(void) {
  __builtin_memset(syscalls, 0, sizeof(syscalls));
  define_syscall(close);
  define_syscall(_exit);
  define_syscall(fcntl);
  define_syscall(getcwd);
  define_syscall(getegid);
  define_syscall(geteuid);
  define_syscall(getgid);
  define_syscall(getpgid);
  define_syscall(getpid);
  define_syscall(getppid);
  define_syscall(getuid);
  define_syscall(isatty);
  define_syscall(open);
  define_syscall(read);
  define_syscall(sbrk);
  define_syscall(setpgid);
  define_syscall(sigaction);
  define_syscall(sigfillset);
  define_syscall(tcgetpgrp);
  define_syscall(tcsetpgrp);
  define_syscall(waitpid);
  define_syscall(write);
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(struct isr_frame* frame) {
  const uintptr_t syscall = frame->eax;
  if (syscall > sizeof(syscalls) || syscalls[syscall].func == NULL) {
    panic("SYSCALL: Syscall %2ld is ENOSYS\n", syscall);
    frame->eax = -38;  // ENOSYS
    return;
  }
  kprintf("SYSCALL: Syscall %2ld %s\n", syscall, syscalls[syscall].name);
  syscalls[syscall].func(frame);
}
