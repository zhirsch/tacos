#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [GETCWD]", __VA_ARGS__)

void syscall_getcwd(struct isr_frame* frame) {
  syscall_in2(frame, char*, buf, "%8p", size_t, size, "%ld");

  if (size < __builtin_strlen(current_process->cwd) + 1) {
    syscall_out(frame, -ERANGE, "%ld");
    return;
  }
  __builtin_strncpy(buf, current_process->cwd, size);
  syscall_out(frame, buf, "%08lx");
}
