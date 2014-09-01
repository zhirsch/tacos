#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "screen.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [WRITE]", __VA_ARGS__)

void syscall_write(struct isr_frame* frame) {
  syscall_in3(frame, int, fd, "%d", const char*, buf, "%8p", size_t, count, "%lx");

  for (uint32_t i = 0; i < count; i++) {
    screen_writech(buf[i]);
  }

  syscall_out(frame, count, "%lx");
}
