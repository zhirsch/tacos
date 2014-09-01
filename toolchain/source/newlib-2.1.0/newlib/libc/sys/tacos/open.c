#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tacos/syscall.h>

#define __SYSCALL__open __SYSCALL_open

_syscall3(int, _open, const char*, name, int, flags, mode_t, mode);

int open(const char* name, int flags, ...) {
  return _open(name, flags, 0);
}
