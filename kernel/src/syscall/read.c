#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "panic.h"
#include "screen.h"

static const char CMD[] = "pwd\n";
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
  kprintf("READ: fd=%ld, buf=%08lx, size=%08lx\n", fd, buf, size);

  if (pos >= __builtin_strlen(CMD) + 1) {
    kprintf("READ: EOF\n");
    frame->eax = 0;
    return;
  }

  size = min(size, __builtin_strlen(CMD) + 1 - pos);
  __builtin_memcpy((void*)buf, CMD + pos, size);
  pos += size;

  kprintf("READ: actually read %ld\n", size);
  frame->eax = size;
}
