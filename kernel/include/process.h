#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include "bits/signal.h"
#include "bits/types.h"

#include "tss.h"

#define FD_STDIN  0
#define FD_STDOUT 1
#define FD_STDERR 2
#define NUM_FDS 16

#define PGID_NONE 0

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
  struct tty* tty;

  // The file descriptor table.
  struct {
    enum {
      PROCESS_FD_CLOSED,
      PROCESS_FD_FILE,
      PROCESS_FD_TTY,
    } type;
    union {
      struct file* file;
      struct tty*  tty;
    } u;
    mode_t mode;
  } fds[NUM_FDS];

  // The signal actions.
  struct sigaction sigactions[NSIG];

  int argc;
  char** argv;
  char** envp;

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

pid_t process_next_pid(void);
pid_t process_next_pgid(void);

#endif /* PROCESS_H */
