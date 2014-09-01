#include <tacos/syscall.h>
#include <unistd.h>

_syscall1(pid_t, getpgid, pid_t, pid);
