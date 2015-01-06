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

  // The location of the program break, where the heap grows from.
  uintptr_t program_break;

  // The list of all children of this process.
  struct child {
    struct process* child;
    struct child* next;
  }* children;
};

// The current running process.
extern struct process* current_process;

pid_t process_next_pid(void);
pid_t process_next_pgid(void);

int process_fork(struct process** child, struct isr_frame* parent_frame);

void switch_to_ring3(uint16_t cs, uint32_t eip, uint16_t ss, uint32_t esp, uint32_t eflags) __attribute__ ((noreturn));

#endif /* PROCESS_H */
