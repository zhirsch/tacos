#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(int, tcsetpgrp, int, fd, pid_t, pgrp);
