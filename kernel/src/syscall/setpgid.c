#include "syscalls/syscalls.h"

#include "bits/types.h"

#include "log.h"
#include "process.h"

#define PANIC(...) panic("SYSCALL [SETPGID]", __VA_ARGS__)

int sys_setpgid(pid_t pid, pid_t pgid) {
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
  return 0;
}
