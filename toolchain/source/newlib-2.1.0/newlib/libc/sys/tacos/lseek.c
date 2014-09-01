#include <sys/types.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall3(off_t, lseek, int, fd, off_t, offset, int, whence);
