#include "process.h"

#include <stddef.h>

#include "log.h"
#include "bits/fcntl.h"

#include "file.h"
#include "interrupts.h"
#include "log.h"
#include "mmu/common.h"
#include "mmu/heap.h"
#include "scheduler.h"
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

int process_fork(struct process** childp, struct isr_frame* parent_frame) {
  struct process* child = NULL;

  child = *childp = kmalloc(sizeof(*child));
  memcpy(child, current_process, sizeof(*child));

  // Set the child process's pid and ppid.
  child->pid = process_next_pid();
  child->ppid = current_process->pid;

  // Set the child process's state.
  child->state = PROCESS_ALIVE;

  // Set the child process's wait state.
  child->wait_state = PROCESS_WAIT_NONE;
  child->status = 0;

  // The child process has no children.
  child->children = NULL;

  // Set the tss.
  memset(&child->tss, 0, sizeof(child->tss));
  child->tss.eip = parent_frame->user_eip;
  child->tss.eflags = parent_frame->user_eflags;
  child->tss.eax = parent_frame->eax;
  child->tss.ecx = parent_frame->ecx;
  child->tss.edx = parent_frame->edx;
  child->tss.ebx = parent_frame->ebx;
  child->tss.esp = parent_frame->user_esp;
  child->tss.ebp = parent_frame->ebp;
  child->tss.esi = parent_frame->esi;
  child->tss.edi = parent_frame->edi;
  child->tss.cs = parent_frame->user_cs;
  child->tss.ss = parent_frame->user_ss;
  child->tss.ds = parent_frame->user_ss;
  child->tss.es = parent_frame->user_ss;
  child->tss.fs = parent_frame->user_ss;
  child->tss.gs = parent_frame->user_ss;

  // The child process starts when the scheduler decides to run it.  When that
  // happens, process_switch is called with it's tss, so the child process's tss
  // and kernel stack (esp0) must look like process_switch had previously
  // switched away from it:
  //
  //   return address
  //   caller's esp
  //   pusha
  //   pointer to process struct
  //
  // The pusha and caller's esp don't matter because there's neither a caller
  // nor previous state for this child process.  The return address is
  // process_start, because the child process is being run for the first time.
  //
  // Below the emulated stack for process_switch, the stack is set up for a
  // "call" to process_start.  The stack looks like:
  //
  //   child's tss (arg1)
  //   return address
  //
  // Since process_start never returns, the return address is unimportant.
  {
    // Create the kernel stack for the child process.
    // TODO: Align on page boundary.
    uint32_t* esp0 = (uint32_t*)kmalloc(0x4000) + 0x4000 / sizeof(uint32_t);

    // process_start's stack
    *(--esp0) = (uintptr_t)&child->tss;    // arg1
    *(--esp0) = 0;                         // ret addr

    // process_switch's stack
    *(--esp0) = (uintptr_t)process_start;  // ret addr
    *(--esp0) = 0;                         // caller's ebp
    for (int i = 0; i < 8; i++) {          // pusha
      *(--esp0) = 0;
    }
    *(--esp0) = (uintptr_t)child;          // pointer to child process

    // Set esp0 in the tss.
    child->tss.ss0 = SEGMENT_KERNEL_DATA;
    child->tss.esp0 = (uintptr_t)esp0;
  }

  // Create a copy of the current address space.
  child->tss.cr3 = mmu_clone_address_space();

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
    if (child->fds[i].type == PROCESS_FD_CLOSED) {
      continue;
    }
    if (child->fds[i].type == PROCESS_FD_FILE) {
      child->fds[i].u.file->ref++;
    }
  }

  // Add the child to the list of the parent's children.
  {
    struct child* node = kmalloc(sizeof(*node));
    node->child = child;
    node->next = current_process->children;
    current_process->children = node;
  }

  // Add the child to the list of processes.
  child->next = current_process->next;
  current_process->next = child;

  return 0;
}
