#include <sys/types.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall0(uid_t, geteuid);
