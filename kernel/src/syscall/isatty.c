#include "syscalls/syscalls.h"

int sys_isatty(int fd) {
  // TODO(zhirsch): Better implementation...
  return (fd == 0 || fd == 1 || fd == 2);
}
