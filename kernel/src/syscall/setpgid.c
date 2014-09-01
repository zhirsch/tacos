#include "syscall/syscalls.h"

#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"

#define LOG(...) log("SYSCALL[SETPGID]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL[SETPGID]", __VA_ARGS__)

void syscall_setpgid(struct isr_frame* frame) {
  pid_t pid = (pid_t)frame->ebx;
  pid_t pgid = (pid_t)frame->ecx;
  LOG("pid=%d, pgid=%d\n", pid, pgid);

  if (pid == 0) {
    pid = current_process->pid;
  }
  if (pid != current_process->pid) {
    PANIC("SETPGID: UNIMPLEMENTED pid is not the current process's pid\n");
  }

  if (pgid == 0) {
    pgid = pid;
  }
  if (pgid != pid) {
    PANIC("SETPGID: UNIMPLEMENTED pgid is not the current process's pid\n");
  }

  current_process->pgid = pgid;
  frame->eax = 0;
}
