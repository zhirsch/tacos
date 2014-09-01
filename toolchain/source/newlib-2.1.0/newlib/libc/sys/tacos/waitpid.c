#include <sys/types.h>
#include <sys/wait.h>
#include <tacos/syscall.h>

_syscall3(pid_t, waitpid, pid_t, pid, int*, status, int, options);
