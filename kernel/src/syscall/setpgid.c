#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [SETPGID]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [SETPGID]", __VA_ARGS__)

void syscall_setpgid(struct isr_frame* frame) {
  syscall_in2(frame, pid_t, pid, "%d", pid_t, pgid, "%d");

  if (pid == 0) {
    pid = current_process->pid;
  }
  if (pid != current_process->pid) {
    PANIC("UNIMPLEMENTED pid is not the current process's pid\n");
  }

  if (pgid == 0) {
    pgid = pid;
  }
  if (pgid != pid) {
    PANIC("UNIMPLEMENTED pgid is not the current process's pid\n");
  }

  current_process->pgid = pgid;
  syscall_out(frame, 0, "%ld");
}
