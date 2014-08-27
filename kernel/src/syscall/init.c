#include "syscall/init.h"

#include "interrupts.h"
#include "panic.h"
#include "syscall/_exit.h"
#include "tss.h"

static void syscall_handler(int vector, int error_code, struct tss* prev_tss);

void init_syscalls(void) {
  interrupt_register_handler(0xFF, syscall_handler);
}

static void syscall_handler(int vector, int error_code, struct tss* prev_tss) {
  switch (prev_tss->eax) {
  case SYSCALL__EXIT:
    syscall__exit(prev_tss);
    break;
  default:
    panic("Unknown syscall %d\n", prev_tss->eax);
  }
}
