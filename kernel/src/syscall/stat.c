#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/fcntl.h"
#include "bits/stat.h"

#include "file.h"
#include "iso9660.h"

int sys_stat(const char* path, struct stat* buf) {
  struct file* file = NULL;
  int err;
  if (buf == NULL) {
    return -EFAULT;
  }
  err = iso9660_open(path, O_RDONLY, &file);
  if (err != 0) {
    return err;
  }
  err = iso9660_fstat(file, buf);
  if (err != 0) {
    iso9660_close(file);
    return err;
  }
  return iso9660_close(file);
}
