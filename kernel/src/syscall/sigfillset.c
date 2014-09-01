#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"
#include "bits/signal.h"

#include "interrupts.h"
#include "log.h"
#include "string.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [SIGFILLSET]", __VA_ARGS__)

void syscall_sigfillset(struct isr_frame* frame) {
  // Printf's %p needs a void*, so this work-around is needed to make GCC happy.
  sigset_t* set;
  syscall_in1(frame, void*, void_set, "%8p");
  set = (sigset_t*)void_set;

  if (set == NULL) {
    syscall_out(frame, -EFAULT, "%ld");
    return;
  }

  *set = (sigset_t)-1;
  syscall_out(frame, 0, "%ld");
}
