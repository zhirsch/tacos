#include "process.h"

#include <stddef.h>

#include "log.h"

#define LOG(...) log("PROCESS", __VA_ARGS__)
#define PANIC(...) panic("PROCESS", __VA_ARGS__)

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
