#include <dirent.h>
#include <sys/types.h>
#include <tacos/syscall.h>

_syscall1(int, closedir, DIR*, dirp);
