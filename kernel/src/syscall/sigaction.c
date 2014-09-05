#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/signal.h"

#include "process.h"

int sys_sigaction(int signum, const struct sigaction* act,
                  struct sigaction* oldact) {
  if (signum <= 0 || signum >= NSIG) {
    return -EINVAL;
  }
  if (signum == SIGKILL || signum == SIGSTOP) {
    return -EINVAL;
  }

  if (oldact != NULL) {
    *oldact = current_process->sigactions[signum];
  }
  if (act != NULL) {
    current_process->sigactions[signum] = *act;
  }

  return 0;
}
