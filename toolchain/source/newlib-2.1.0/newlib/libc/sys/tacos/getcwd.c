#include <stdint.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall2(char*, getcwd, char*, buf, size_t, size);
