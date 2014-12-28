#include "syscall.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/syscall.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscalls/syscalls.h"

#define PANIC(...) panic("SYSCALL", __VA_ARGS__)

syscallfn syscalls[NSYSCALLS];

static void syscall_handler(struct isr_frame* frame);

void init_syscalls(void) {
  init_syscalls_array();
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(struct isr_frame* frame) {
  const uintptr_t syscall = frame->eax;
  if (syscall >= NSYSCALLS || syscalls[syscall] == NULL) {
    PANIC("Unknown eax=%2ld\n", syscall);
    frame->eax = -ENOSYS;
    return;
  }
  syscalls[syscall](frame);
}
