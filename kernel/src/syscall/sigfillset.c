#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/signal.h"

int sys_sigfillset(sigset_t* set) {
  if (set == NULL) {
    return -EFAULT;
  }

  *set = (sigset_t)-1;
  return 0;
}
