#include "syscall/init.h"

#include "interrupts.h"
#include "panic.h"
#include "syscall/syscalls.h"

const char* syscall_names[NUM_SYSCALLS] = {
  "_exit",  // 0x00
  "write",  // 0x01
  "sbrk",   // 0x02
};

static void syscall_handler(struct isr_frame* frame);

void init_syscalls(void) {
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(struct isr_frame* frame) {
  const int syscall = frame->eax;
  kprintf("SYSCALL: Handling %s (%02x)\n", syscall_names[syscall], syscall);
  switch ((enum syscalls)syscall) {
  case SYSCALL__EXIT:
    syscall__exit(frame);
    return;
  case SYSCALL_WRITE:
    syscall_write(frame);
    return;
  case SYSCALL_SBRK:
    syscall_sbrk(frame);
    return;
  }
  panic("SYSCALL: Unknown syscall %d\n", syscall);
}
