#include "syscall.h"

#include <stddef.h>

#include "bits/syscall.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"

#define LOG(...) log("SYSCALL", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL", __VA_ARGS__)

#define define_syscall(n)                                               \
  do {                                                                  \
    extern void syscall_##n(struct isr_frame* frame);                   \
    syscalls[__SYSCALL_##n].name = #n;                                  \
    syscalls[__SYSCALL_##n].func = syscall_##n;                         \
  } while (0)

static struct {
  const char* name;
  void (*func)(struct isr_frame* frame);
} syscalls[42];

static void syscall_handler(struct isr_frame* frame);

void init_syscalls(void) {
  define_syscall(_exit);
  define_syscall(close);
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
  define_syscall(stat);
  define_syscall(tcgetpgrp);
  define_syscall(tcsetpgrp);
  define_syscall(waitpid);
  define_syscall(write);

  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(struct isr_frame* frame) {
  const uintptr_t syscall = frame->eax;
  if (syscall > sizeof(syscalls) || syscalls[syscall].func == NULL) {
    PANIC("Unknown eax=%2ld\n", syscall);
    frame->eax = -38;  // ENOSYS
    return;
  }
  LOG("Handling %s\n", syscalls[syscall].name);
  syscalls[syscall].func(frame);
}
