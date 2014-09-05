#include "syscalls/syscalls.h"

#include <stddef.h>
#include <stdint.h>

#include "bits/errno.h"

#include "screen.h"

ssize_t sys_write(int fd, const void* buf, size_t count) {
  if (buf == NULL) {
    return -EFAULT;
  }
  for (size_t i = 0; i < count; i++) {
    screen_writech(((char*)buf)[i]);
  }
  return count;
}
