#include <signal.h>
#include <tacos/syscall.h>

_syscall1(int, sigsuspend, const sigset_t*, mask);
