#include <tacos/syscall.h>
#include <unistd.h>

_syscall1(int, chdir, const char*, path)
