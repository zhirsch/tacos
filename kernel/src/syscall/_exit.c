#include "syscalls/syscalls.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "scheduler.h"

#define LOG(...) log("SYSCALL [_exit]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [_exit]", __VA_ARGS__)

void sys__exit(int status, struct isr_frame* frame) {
  LOG("pid %d terminated with status %d\n", current_process->pid, status);

  // Set the process as a zombie.
  current_process->state = PROCESS_ZOMBIE;

  // Allow waitpid to work for this process.
  current_process->wait_state = PROCESS_WAIT_EXITED;
  current_process->status = status;

  // Yield to something else, this process is dead.
  scheduler_yield();
  PANIC("scheduler_yield returned to _exit\n");
}
