#include <signal.h>
#include <tacos/syscall.h>

_syscall3(int, sigprocmask, int, how, const sigset_t*, set, sigset_t*, oldset);
