#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#include "bits/signal.h"
#include "bits/types.h"

#include "interrupts.h"
#include "tss.h"

#define FD_STDIN  0
#define FD_STDOUT 1
#define FD_STDERR 2
#define NUM_FDS 16

#define PGID_NONE 0

#define SEGMENT_KERNEL_CODE ((1 << 3) | 0x0)
#define SEGMENT_KERNEL_DATA ((2 << 3) | 0x0)
#define SEGMENT_USER_CODE   ((3 << 3) | 0x3)
#define SEGMENT_USER_DATA   ((4 << 3) | 0x3)

struct process {
  // The TSS for storing the interrupted state of the process.
  struct tss tss;

  // The pids of this process, its parent, and its process group.
  pid_t pid;
  pid_t ppid;
  pid_t pgid;

  // The user that this process is running as.
  uid_t uid;
  uid_t euid;
  gid_t gid;
  gid_t egid;

  // The scheduling state of the process.
  enum {
    PROCESS_ALIVE,
    PROCESS_ZOMBIE,
  } state;

  // The state of the process for waitpid(2).
  enum {
    PROCESS_WAIT_NONE,
    PROCESS_WAIT_EXITED,
    PROCESS_WAIT_STOPPED,
    PROCESS_WAIT_CONTINUED,
  } wait_state;

  // The status of the process.  See waitpid(2).
  int status;

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

  // The location of the program break, where the heap grows from.
  uintptr_t program_break;

  // The list of all children of this process.
  struct child {
    struct process* child;
    struct child* next;
  }* children;

  // The next process in the system.
  struct process* next;
};

// The current running process.
extern struct process* current_process;

pid_t process_next_pid(void);
pid_t process_next_pgid(void);

typedef void(*entry_point_t)() __attribute__((noreturn));

struct process* process_create(entry_point_t entry, struct process* template, ...);

int process_fork(struct process** child, struct isr_frame* parent_frame);

// Starts the process by switching to ring3 at the eip configured in the tss.
void process_start(void) __attribute__ ((noreturn));

// Switches to a new process.  When this returns, the new process will be
// running in kernel mode.
void process_switch(struct tss* old_tss, struct tss* new_tss);

#endif /* PROCESS_H */
