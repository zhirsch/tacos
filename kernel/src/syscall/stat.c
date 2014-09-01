#include <stddef.h>

#include "bits/errno.h"
#include "bits/stat.h"

#include "interrupts.h"
#include "log.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [STAT]", __VA_ARGS__)

void syscall_stat(struct isr_frame* frame) {
  struct stat* buf;
  syscall_in2(frame, const char*, path, "%s", void*, void_buf, "%8p");
  buf = (struct stat*)void_buf;

  if (buf == NULL) {
    syscall_out(frame, -EFAULT, "%ld");
    return;
  }

  // TODO
  buf->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IXUSR;

  syscall_out(frame, 0, "%ld");
}
