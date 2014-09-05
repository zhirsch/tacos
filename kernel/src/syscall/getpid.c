#include "syscalls/syscalls.h"

#include "process.h"

pid_t sys_getpid(void) {
  return current_process->pid;
}
