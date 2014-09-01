#include <tacos/syscall.h>
#include <unistd.h>

_syscall1(int, pipe, int, pipefd[2]);
