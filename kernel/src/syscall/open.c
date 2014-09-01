#include <stdint.h>

#include "bits/errno.h"
#include "bits/types.h"

#include "interrupts.h"
#include "log.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [OPEN]", __VA_ARGS__)

void syscall_open(struct isr_frame* frame) {
  syscall_in3(frame, const char*, pathname, "%s", int, flags, "%08x", mode_t, mode, "%08x");
  syscall_out(frame, -ENOENT, "%ld");
}
