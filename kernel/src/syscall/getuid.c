#include "syscalls/syscalls.h"

#include "process.h"

uid_t sys_getuid(void) {
  return current_process->uid;
}
