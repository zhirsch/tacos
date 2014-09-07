#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

pid_t sys_fork(void) {
  return -EAGAIN;
}
