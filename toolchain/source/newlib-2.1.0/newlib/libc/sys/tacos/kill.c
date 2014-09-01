#include <signal.h>
#include <sys/types.h>
#include <tacos/syscall.h>

_syscall2(int, kill, pid_t, pid, int, sig);
