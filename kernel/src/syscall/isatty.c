#include "interrupts.h"
#include "log.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [ISATTY]", __VA_ARGS__)

void syscall_isatty(struct isr_frame* frame) {
  syscall_in1(frame, int, fd, "%d");
  // TODO(zhirsch): Better implementation...
  syscall_out(frame, (fd == 0 || fd == 1 || fd == 2), "%ld");
}
