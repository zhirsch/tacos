#include <tacos/syscall.h>
#include <unistd.h>

_syscall3(int, write, int, fd, const void*, buf, size_t, count);
