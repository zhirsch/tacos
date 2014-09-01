#include "syscall/syscalls.h"

#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "process.h"

#define LOG(...) log("SYSCALL[CLOSE]", __VA_ARGS__)

void syscall_close(struct isr_frame* frame) {
  const int fd = (int)frame->ebx;
  LOG("fd=%d\n", fd);

  if (fd > NUM_FDS) {
    LOG("Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    LOG("Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  current_process->fds[fd].used = 0;
  frame->eax = 0;
}
