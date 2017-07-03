#include "scheduler.h"

#include "log.h"
#include "process.h"

#define LOG(...) log("SCHEDULER", __VA_ARGS__)
#define PANIC(...) panic("SCHEDULER", __VA_ARGS__)

void scheduler_yield(void) {
  struct process* next = NULL;
  assert(current_process != NULL);

  // Find the next runnable process and switch to it.
  next = current_process->next;
  while (next != current_process) {
    assert(next != NULL);
    if (next->state == PROCESS_ALIVE) {
      LOG("pid %d yields to pid %d\n", current_process->pid, next->pid);
      process_switch(&current_process->tss, &next->tss);
      LOG("pid %d returns from pid %d\n", current_process->pid, next->pid);
      return;
    }
    next = next->next;
  }

  // This should only happen when the current process is the idle process.
  // LOG("no process to yield to\n");
}
