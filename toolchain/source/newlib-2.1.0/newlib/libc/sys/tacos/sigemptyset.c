#include <signal.h>
#include <tacos/syscall.h>

_syscall1(int, sigemptyset, sigset_t*, set);
