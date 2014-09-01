#include <signal.h>
#include <tacos/syscall.h>

_syscall3(int, sigaction, int, signum, const struct sigaction*, act,
          struct sigaction*, oldact);
