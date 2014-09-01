#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "screen.h"

#define LOG(...) log("SYSCALL[READ]", __VA_ARGS__)

static const char CMD[] = "env\n";
static unsigned int pos = 0;

static int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

void syscall_read(struct isr_frame* frame) {
  const uint32_t fd = frame->ebx;
  const uintptr_t buf = frame->ecx;
  uint32_t size = frame->edx;
  LOG("fd=%ld, buf=%08lx, size=%08lx\n", fd, buf, size);

  if (pos >= __builtin_strlen(CMD) + 1) {
    LOG("EOF\n");
    frame->eax = 0;
    return;
  }

  size = min(size, __builtin_strlen(CMD) + 1 - pos);
  __builtin_memcpy((void*)buf, CMD + pos, size);
  pos += size;

  LOG("actually read %ld\n", size);
  frame->eax = size;
}
