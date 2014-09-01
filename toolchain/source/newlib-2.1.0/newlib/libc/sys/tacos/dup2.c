#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(int, dup2, int, oldfd, int, newfd);
