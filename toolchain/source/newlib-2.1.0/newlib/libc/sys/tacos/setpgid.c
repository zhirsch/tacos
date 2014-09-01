#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(int, setpgid, pid_t, pid, pid_t, pgid);
