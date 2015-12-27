#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/types.h"
#include "bits/wait.h"

#include "interrupts.h"
#include "log.h"
#include "mmu/common.h"
#include "mmu/heap.h"
#include "process.h"
#include "scheduler.h"

#define LOG(...) log("SYSCALL [waitpid]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [waitpid]", __VA_ARGS__)

static pid_t wait_for_child_in_pgid(pid_t pgid, int* status, int options);
static pid_t wait_for_any_child(int* status, int options);
static pid_t wait_for_child(pid_t pid, int* status, int options);

pid_t sys_waitpid(pid_t pid, int* status, int options, struct isr_frame* frame) {
  if (pid < -1) {
    return wait_for_child_in_pgid(-pid, status, options);
  }
  if (pid == -1) {
    return wait_for_any_child(status, options);
  }
  if (pid == 0) {
    return wait_for_child_in_pgid(current_process->pgid, status, options);
  }
  return wait_for_child(pid, status, options);
}

static pid_t inspect_child_process(struct process* child, int* status, int options) {
  const pid_t pid = child->pid;

  switch (child->wait_state) {
  case PROCESS_WAIT_EXITED:
    if (child->state != PROCESS_ZOMBIE) {
      PANIC("PROCESS_WAIT_EXITED is not a zombie\n");
    }
    if (status != NULL) {
      *status = child->status;
    }

    // Remove the child process from the parent's list of children.
    {
      struct child* prev = NULL;
      struct child* this = current_process->children;
      assert(this != NULL);
      while (this->child != child) {
        prev = this;
        this = this->next;
      }
      if (prev == NULL) {
        current_process->children = this->next;
      } else {
        prev->next = this->next;
      }
    }

    // Free the pages in the address space.
    mmu_free_address_space(child->tss.cr3);

    // Free the struct process object.
    kfree(child);
    return pid;

  case PROCESS_WAIT_STOPPED:
    if (options & WUNTRACED) {
      if (status != NULL) {
        *status = child->status;
      }
      child->wait_state = PROCESS_WAIT_NONE;
      return child->pid;
    }
    break;

  case PROCESS_WAIT_CONTINUED:
    if (options & WCONTINUED) {
      if (status != NULL) {
        *status = child->status;
      }
      child->wait_state = PROCESS_WAIT_NONE;
      return child->pid;
    }
    break;

  case PROCESS_WAIT_NONE:
    break;
  }

  return 0;
}

static pid_t wait_for_child_in_pgid(pid_t pgid, int* status, int options) {
  if (current_process->children == NULL) {
    return -ECHILD;
  }
  while (1) {
    bool found = false;
    for (struct child* it = current_process->children; it != NULL; it = it->next) {
      pid_t pid;
      if (it->child->pgid != pgid) {
        continue;
      }
      found = true;
      pid = inspect_child_process(it->child, status, options);
      if (pid > 0) {
        return pid;
      }
    }
    if (!found) {
      return -ECHILD;
    }
    if (options & WNOHANG) {
      return 0;
    }
    scheduler_yield();
  }
}

static pid_t wait_for_any_child(int* status, int options) {
  if (current_process->children == NULL) {
    return -ECHILD;
  }
  while (1) {
    for (struct child* it = current_process->children; it != NULL; it = it->next) {
      const pid_t pid = inspect_child_process(it->child, status, options);
      if (pid > 0) {
        return pid;
      }
    }
    if (options & WNOHANG) {
      return 0;
    }
    scheduler_yield();
  }
}

static pid_t wait_for_child(pid_t pid, int* status, int options) {
  struct process* child = NULL;
  for (struct child* it = current_process->children; it != NULL; it = it->next) {
    if (it->child->pid == pid) {
      child = it->child;
      break;
    }
  }
  if (child == NULL) {
    return -ECHILD;
  }
  while (1) {
    const pid_t pid = inspect_child_process(child, status, options);
    if (pid > 0) {
      return pid;
    }
    if (options & WNOHANG) {
      return 0;
    }
    scheduler_yield();
  }
}
