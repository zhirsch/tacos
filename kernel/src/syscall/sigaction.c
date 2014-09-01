#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"
#include "bits/signal.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [SIGACTION]", __VA_ARGS__)

void syscall_sigaction(struct isr_frame* frame) {
  // Printf's %p needs a void*, so this work-around is needed to make GCC happy.
  const struct sigaction* act;
  struct sigaction* oldact;
  syscall_in3(frame, int, signum, "%d", const void*, void_act, "%8p", void*, void_oldact, "%8p");
  act = (const struct sigaction*)void_act;
  oldact = (struct sigaction*)void_oldact;

  if (signum <= 0 || signum >= NUM_SIGNALS) {
    syscall_out(frame, -EINVAL, "%ld");
    return;
  }
  if (signum == SIGKILL || signum == SIGSTOP) {
    syscall_out(frame, -EINVAL, "%ld");
    return;
  }

  if (oldact != NULL) {
    __builtin_memcpy(oldact, current_process->sigactions + signum, sizeof(*oldact));
  }
  if (act != NULL) {
    __builtin_memcpy(current_process->sigactions + signum, act, sizeof(*act));
  }
  syscall_out(frame, 0, "%ld");
}
