#include <tacos/syscall.h>
#include <unistd.h>

_syscall3(int, execve, const char*, filename, char* const*, argv,
          char* const*, envp);
