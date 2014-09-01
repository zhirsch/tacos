#include "interrupts.h"
#include "log.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [_EXIT]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [_EXIT]", __VA_ARGS__)

void syscall__exit(struct isr_frame* frame) {
  syscall_in1(frame, int, status, "%d");
  PANIC("Process terminated with status %d\n", status);
}
