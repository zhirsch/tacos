#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

int sys_sigsuspend(const sigset_t* mask) {
  return -ENOSYS;
}
