#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "log.h"

#define PANIC(...) panic("SYSCALL [_EXIT]", __VA_ARGS__)

void sys__exit(int status) {
  PANIC("Process terminated with status %d\n", status);
}
