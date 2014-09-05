#include "syscalls/syscalls.h"

#include "process.h"

gid_t sys_getgid(void) {
  return current_process->gid;
}
