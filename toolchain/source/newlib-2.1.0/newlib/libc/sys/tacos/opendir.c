#include <dirent.h>
#include <sys/types.h>
#include <tacos/syscall.h>

_syscall1(DIR*, opendir, const char*, name);
