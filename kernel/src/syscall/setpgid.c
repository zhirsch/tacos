#include "syscall/syscalls.h"

#include "bits/types.h"

#include "interrupts.h"
#include "kprintf.h"
#include "panic.h"
#include "process.h"

void syscall_setpgid(struct isr_frame* frame) {
  pid_t pid = (pid_t)frame->ebx;
  pid_t pgid = (pid_t)frame->ecx;
  kprintf("SETPGID: pid=%d, pgid=%d\n", pid, pgid);

  if (pid == 0) {
    pid = current_process->pid;
  }
  if (pid != current_process->pid) {
    panic("SETPGID: UNIMPLEMENTED pid is not the current process's pid\n");
  }

  if (pgid == 0) {
    pgid = pid;
  }
  if (pgid != pid) {
    panic("SETPGID: UNIMPLEMENTED pgid is not the current process's pid\n");
  }

  current_process->pgid = pgid;
  frame->eax = 0;
}
