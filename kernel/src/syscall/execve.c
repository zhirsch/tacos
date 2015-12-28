#include "syscalls/syscalls.h"

#include "bits/errno.h"

#include "interrupts.h"
#include "log.h"

int sys_execve(const char* filename, char* const argv[], char* const envp[],
               struct isr_frame* frame) {
  return -ENOENT;
}
