#include "syscalls/syscalls.h"

#include "process.h"

gid_t sys_getegid(void) {
  return current_process->egid;
}
