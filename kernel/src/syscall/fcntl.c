#include "syscall/syscalls.h"

#include <stdint.h>

#include "bits/fcntl.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"

#define LOG(...) log("SYSCALL[FCNTL]", __VA_ARGS__)

static int dupfd(int oldfd, int newfd);
static int setfd(int fd, int val);

void syscall_fcntl(struct isr_frame* frame) {
  const int fd = frame->ebx;
  const int cmd = frame->ecx;
  const uint32_t arg = frame->edx;

  LOG("fd=%d, cmd=%08x, arg=%08lx\n", fd, cmd, arg);

  if (fd < 0 || fd > NUM_FDS) {
    LOG("Invalid fd\n");
    frame->eax = -9;  // EBADF
    return;
  }
  if (!current_process->fds[fd].used) {
    LOG("Unopened fd\n");
    frame->eax = -9;  // EBADF
    return;
  }

  switch (cmd) {
  case F_DUPFD:
    frame->eax = dupfd(fd, arg);
    return;
  case F_SETFD:
    frame->eax = setfd(fd, arg);
    return;
  default:
    frame->eax = -34;  // ERANGE
    return;
  }
}

static int dupfd(int oldfd, int newfd) {
  for (int i = newfd; i < NUM_FDS; i++) {
    if (!current_process->fds[i].used) {
      __builtin_memcpy(current_process->fds + i, current_process->fds + oldfd,
                       sizeof(current_process->fds[i]));
      LOG("F_DUPFD %d to %d\n", oldfd, i);
      return i;
    }
  }
  LOG("F_DUPFD is EMFILE\n");
  return -24;  // EMFILE
}

static int setfd(int fd, int val) {
  switch (val) {
  case FD_CLOEXEC:
    LOG("Set cloexec for %d\n", fd);
    current_process->fds[fd].cloexec = 1;
    return 0;
  default:
    return -34;  // ERANGE
  }
}
