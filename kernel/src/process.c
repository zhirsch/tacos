#include "process.h"

#include <stddef.h>

#include "kmalloc.h"
#include "log.h"
#include "mmu.h"

#define LOG(...) log("MMU", __VA_ARGS__)
#define PANIC(...) panic("MMU", __VA_ARGS__)

struct process* current_process = NULL;

pid_t process_next_pid(void) {
  static pid_t next_pid = 1;
  if (next_pid != 1) {
    PANIC("Can't have more than one process yet\n");
  }
  return next_pid++;
}

pid_t process_next_pgid(void) {
  static pid_t next_pgid = 1;
  if (next_pgid != 1) {
    PANIC("Can't have more than one process group yet\n");
  }
  return next_pgid++;
}
