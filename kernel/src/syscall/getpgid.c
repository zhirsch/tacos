#include "syscalls/syscalls.h"

#include "process.h"

pid_t sys_getpgid(void) {
  return current_process->pgid;
}
