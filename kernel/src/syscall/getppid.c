#include "syscalls/syscalls.h"

#include "process.h"

pid_t sys_getppid(void) {
  return current_process->ppid;
}
