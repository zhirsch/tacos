#include "process.h"

#include <stddef.h>

#include "log.h"
#include "bits/fcntl.h"

#include "file.h"
#include "log.h"
#include "mmu/common.h"
#include "mmu/heap.h"
#include "string.h"

#define LOG(...) log("PROCESS", __VA_ARGS__)
#define PANIC(...) panic("PROCESS", __VA_ARGS__)

struct process* current_process = NULL;

pid_t process_next_pid(void) {
  static pid_t next_pid = 1;
  return next_pid++;
}

pid_t process_next_pgid(void) {
  static pid_t next_pgid = 1;
  return next_pgid++;
}

int process_fork(struct process** child) {
  *child = kmalloc(sizeof(**child));
  memcpy(*child, current_process, sizeof(**child));

  (*child)->pid = process_next_pid();
  (*child)->ppid = current_process->pid;
  (*child)->children = NULL;
  // TODO: Copy all the pages and memory in the process struct.

  // TODO: The child does not inherit its parent's memory locks (mlock(2), mlockall(2)).
  // TODO: Process resource utilizations (getrusage(2)) and CPU time counters (times(2)) are reset to zero in the child.
  // TODO: The child's set of pending signals is initially empty (sigpending(2)).
  // TODO: The child does not inherit semaphore adjustments from its parent (semop(2)).
  // TODO: The child does not inherit record locks from its parent (fcntl(2)).
  // TODO: The child does not inherit timers from its parent (setitimer(2), alarm(2), timer_create(2)).
  // TODO: The child does not inherit outstanding asynchronous I/O operations from its  parent  (aio_read(3),  aio_write(3)),  nor
  //       does it inherit any asynchronous I/O contexts from its parent (see io_setup(2)).

  // Increment the reference counts for open files.
  for (int i = 0; i < NUM_FDS; i++) {
    if ((*child)->fds[i].type == PROCESS_FD_CLOSED) {
      continue;
    }
    if ((*child)->fds[i].type == PROCESS_FD_FILE) {
      ++((*child)->fds[i].u.file->ref);
    }
  }

  // Create a copy of the current address space.
  (*child)->tss.cr3 = mmu_clone_address_space();

  // Add the child to the list of the parent's children.
  {
    struct child* node = kmalloc(sizeof(*node));
    node->child = *child;
    node->next = current_process->children;
    current_process->children = node;
  }

  // TODO: Set the child's stack so that it will return to the caller correctly.

  // TODO: Add the child to the scheduler.

  return 0;
}
