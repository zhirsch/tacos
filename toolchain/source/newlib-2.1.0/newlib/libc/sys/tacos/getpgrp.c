#include <tacos/syscall.h>
#include <unistd.h>

pid_t getpgrp(void) {
  return getpgid(getpid());
}
