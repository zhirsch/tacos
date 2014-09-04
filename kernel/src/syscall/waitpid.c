#include <stdint.h>

#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [WAITPID]", __VA_ARGS__)

void syscall_waitpid(struct isr_frame* frame) {
  // Printf's %p needs a void*, so this work-around is needed to make GCC happy.
  //int* status;
  syscall_in3(frame, pid_t, pid, "%ld", void*, void_status, "%8p", int, options, "%08x");
  //status = (int*)void_status;

  // TODO
  syscall_out(frame, 0, "%ld");
}
