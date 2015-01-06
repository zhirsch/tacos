#include "syscalls/syscalls.h"

#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"

int sys_open(const char* pathname, int flags, mode_t mode, struct isr_frame* frame) {
  return -ENOENT;
}
