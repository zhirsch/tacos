#include <signal.h>
#include <tacos/syscall.h>

_syscall1(int, sigfillset, sigset_t*, set);
