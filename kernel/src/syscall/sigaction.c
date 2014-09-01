#include "syscall/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "bits/signal.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"

#define LOG(...) log("SYSCALL[SIGACTION]", __VA_ARGS__)

void syscall_sigaction(struct isr_frame* frame) {
  const uint32_t signum = frame->ebx;
  const struct sigaction* act = (const struct sigaction*)frame->ecx;
  struct sigaction* oldact = (struct sigaction*)frame->edx;

  if (signum <= 0 || signum >= NUM_SIGNALS) {
    LOG("Unknown signal %ld\n", signum);
    frame->eax = -22;  // EINVAL
    return;
  }
  if (signum == SIGKILL || signum == SIGSTOP) {
    LOG("Can't call sigaction for SIGKILL or SIGSTOP\n");
    frame->eax = -22;  // EINVAL
    return;
  }

  if (oldact != NULL) {
    LOG("Returning old sigaction for %ld\n", signum);
    __builtin_memcpy(oldact, current_process->sigactions + signum, sizeof(*oldact));
  }
  if (act != NULL) {
    LOG("Setting sigaction for %ld\n", signum);
    __builtin_memcpy(current_process->sigactions + signum, act, sizeof(*act));
  }
  frame->eax = 0;
}
