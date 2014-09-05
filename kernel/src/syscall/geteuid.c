#include "syscalls/syscalls.h"

#include "process.h"

uid_t sys_geteuid(void) {
  return current_process->euid;
}
