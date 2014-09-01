#include <fcntl.h>
#include <stdarg.h>
#include <tacos/syscall.h>
#include <unistd.h>

#define __SYSCALL__fcntl __SYSCALL_fcntl

_syscall3(int, _fcntl, int, fd, int, cmd, long, arg);

int fcntl(int fd, int cmd, ...) {
  long arg = 0;
  switch (cmd) {
  case F_DUPFD:
  case F_SETFD: {
    va_list ap;
    va_start(ap, cmd);
    arg = va_arg(ap, int);
    va_end(ap);
    break;
  }
  }
  return _fcntl(fd, cmd, arg);
}
