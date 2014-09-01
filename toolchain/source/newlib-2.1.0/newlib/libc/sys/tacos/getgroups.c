#include <sys/types.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(int, getgroups, int, size, gid_t*, list);
