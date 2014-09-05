#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/stat.h"

int sys_stat(const char* path, struct stat* buf) {
  if (buf == NULL) {
    return -EFAULT;
  }

  // TODO
  buf->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IXUSR;
  return 0;
}
