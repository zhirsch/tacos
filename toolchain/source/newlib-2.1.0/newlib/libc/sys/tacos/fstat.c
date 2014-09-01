#include <sys/types.h>
#include <sys/stat.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(int, fstat, int, fd, struct stat*, buf);
