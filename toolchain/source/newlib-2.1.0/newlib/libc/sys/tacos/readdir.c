#include <dirent.h>
#include <tacos/syscall.h>

_syscall1(struct dirent*, readdir, DIR*, dirp);
