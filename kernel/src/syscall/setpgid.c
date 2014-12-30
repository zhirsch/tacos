#include "syscalls/syscalls.h"

#include "bits/types.h"

#include "log.h"
#include "process.h"

#define PANIC(...) panic("SYSCALL [setpgid]", __VA_ARGS__)

/*
       setpgid()  sets  the  PGID  of  the  process specified by pid to pgid.  If pid is zero, then the process ID of the calling
       process is used.  If pgid is zero, then the PGID of the process specified by pid is made the same as its process  ID.   If
       setpgid() is used to move a process from one process group to another (as is done by some shells when creating pipelines),
       both process groups must be part of the same session (see setsid(2) and credentials(7)).  In this case, the pgid specifies
       an existing process group to be joined and the session ID of that group must match the session ID of the joining process.
*/

int sys_setpgid(pid_t pid, pid_t pgid) {
  if (pid == 0) {
    pid = current_process->pid;
  }
  if (pgid == 0) {
    pgid = pid;
  }

#if 0
  if (pgid already exists) {
    verify that the current pg and new pg are in the same session;
    set current pg = new pg;
    return 0;
  }
#endif

  // Create a new process group.
  current_process->pgid = pgid;
  return 0;
}
