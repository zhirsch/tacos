#include <sys/types.h>
#include <sys/stat.h>
#include <tacos/syscall.h>

_syscall1(mode_t, umask, mode_t, mask);
