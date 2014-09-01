#include "syscall/syscalls.h"

#include <stdint.h>

#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "screen.h"

#define LOG(...) log("SYSCALL[WAITPID]", __VA_ARGS__)

void syscall_waitpid(struct isr_frame* frame) {
  const pid_t pid = (pid_t)frame->ebx;
  int* const status = (int*)frame->ecx;
  const int options = (int)frame->edx;
  LOG("pid=%d, status=%08lx, options=%08x\n", pid, (uintptr_t)status, options);
  frame->eax = 0;  // TODO
}
