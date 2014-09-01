#include <tacos/syscall.h>
#include <unistd.h>

_syscall1(int, isatty, int, fd);
