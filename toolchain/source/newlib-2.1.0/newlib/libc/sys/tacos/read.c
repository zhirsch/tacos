#include <tacos/syscall.h>
#include <unistd.h>

_syscall3(int, read, int, fd, void*, buf, size_t, count);
