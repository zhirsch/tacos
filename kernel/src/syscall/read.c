#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "screen.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [READ]", __VA_ARGS__)

static const char CMD[] = "env\n";
static unsigned int pos = 0;

static int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

void syscall_read(struct isr_frame* frame) {
  syscall_in3(frame, int, fd, "%d", void*, buf, "%8p", size_t, size, "%lx");

  if (pos >= __builtin_strlen(CMD) + 1) {
    syscall_out(frame, 0, "%ld");
    return;
  }

  size = min(size, __builtin_strlen(CMD) + 1 - pos);
  __builtin_memcpy((void*)buf, CMD + pos, size);
  pos += size;

  syscall_out(frame, size, "%lx");
}
