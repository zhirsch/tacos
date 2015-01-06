#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"

#include "interrupts.h"
#include "process.h"
#include "string.h"

char* sys_getcwd(char* buf, size_t size, struct isr_frame* frame) {
  if (size < strlen(current_process->cwd) + 1) {
    return (char*)-ERANGE;
  }
  return strncpy(buf, current_process->cwd, size);
}
