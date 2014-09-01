#include <stdint.h>
#include <tacos/syscall.h>
#include <unistd.h>

_syscall1(void*, sbrk, intptr_t, increment);
