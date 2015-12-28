#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/stat.h"

int sys_fstat(int fd, struct stat* buf, struct isr_frame* frame) {
  return -EBADF;
}
