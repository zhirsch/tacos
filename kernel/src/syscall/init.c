#include "syscall/init.h"

#include "interrupts.h"
#include "panic.h"
#include "syscall/syscalls.h"

const char* syscall_names[NUM_SYSCALLS] = {
  "_exit",  // 0x00
  "write",  // 0x01
  "sbrk",   // 0x02
  "getcwd", // 0x03
};

static void syscall_handler(struct isr_frame* frame);

void init_syscalls(void) {
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(struct isr_frame* frame) {
  const int syscall = frame->eax;
  kprintf("SYSCALL: Handling %s (%02x)\n", syscall_names[syscall], syscall);
  switch (syscall) {
  case SYSCALL__EXIT:
    syscall__exit(frame);
    return;
  case SYSCALL_WRITE:
    syscall_write(frame);
    return;
  case SYSCALL_SBRK:
    syscall_sbrk(frame);
    return;
  case SYSCALL_GETCWD:
    syscall_getcwd(frame);
    return;
  default:
    panic("SYSCALL: Unknown syscall %d\n", syscall);
  }
}
