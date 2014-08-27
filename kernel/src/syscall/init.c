#include "syscall/init.h"

#include "interrupts.h"
#include "panic.h"
#include "syscall/_exit.h"
#include "syscall/write.h"
#include "tss.h"

const char* syscall_names[NUM_SYSCALLS] = {
  "_exit",  // 0x00
  "write",  // 0x01
};

static void syscall_handler(int vector, int error_code, struct tss* prev_tss);

void init_syscalls(void) {
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(int vector, int error_code, struct tss* prev_tss) {
  const int syscall = prev_tss->eax;
  kprintf("SYSCALL: Handling %s (%02x)\n", syscall_names[syscall], syscall);
  switch ((enum syscalls)syscall) {
  case SYSCALL__EXIT:
    syscall__exit(prev_tss);
    return;
  case SYSCALL_WRITE:
    syscall_write(prev_tss);
    return;
  }
  panic("SYSCALL: Unknown syscall %d\n", syscall);
}
