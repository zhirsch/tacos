#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include "bits/signal.h"
#include "bits/types.h"

#include "mmu.h"
#include "tss.h"

#define NUM_FDS 16

struct process {
  // The pids of this process, its parent, and its process group.
  pid_t pid;
  pid_t ppid;
  pid_t pgid;

  // The user that this process is running as.
  uid_t uid;
  uid_t euid;
  gid_t gid;
  gid_t egid;

  // The controlling tty for the process.
  int tty;

  // The file descriptor table.
  struct {
    int used;
    int flags;
    int tty;
    int cloexec;
  } fds[NUM_FDS];

  // The signal actions.
  struct sigaction sigactions[NUM_SIGNALS];

  // The current working directory for the process.
  const char* cwd;

  // The TSS for storing the interrupted state of the process.
  struct tss tss;

  // The page that stores the args and environment passed to the process.
  uintptr_t args_paddr;

  // The location of the program break, where the heap grows from.
  uintptr_t program_break;

  // TODO(zhirsch): Include:
  //  * The ELF pages. (when should they be deallocated?)
};

// The current running process.
extern struct process* current_process;

#endif /* PROCESS_H */
