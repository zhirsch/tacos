#include "syscalls/syscalls.h"

#include <stddef.h>

#include "bits/types.h"

#include "string.h"

static const char CMD[] = "env\n";
static unsigned int pos = 0;

static int min(int a, int b) {
  if (a < b) {
    return a;
  }
  return b;
}

ssize_t sys_read(int fd, void* buf, size_t size) {
  if (pos >= strlen(CMD) + 1) {
    return 0;
  }
  size = min(size, strlen(CMD) + 1 - pos);
  memcpy((void*)buf, CMD + pos, size);
  pos += size;
  return size;
}
