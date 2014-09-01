#include "syscall/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "screen.h"

#define LOG(...) log("SYSCALL[WRITE]", __VA_ARGS__)

void syscall_write(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  const char* const buf = (const char*)frame->ecx;
  const size_t count = (size_t)frame->edx;
  LOG("fd=%d, buf=%08lx, count=%08lx\n", fd, (uintptr_t)buf, count);
  for (uint32_t i = 0; i < count; i++) {
    screen_writech(buf[i]);
  }
  frame->eax = count;
}
