#include <tacos/syscall.h>
#include <unistd.h>

void _exit(int status) {
  __asm__ __volatile__ ("int $0xFF"
                        :
                        : "a" (__SYSCALL__exit),
                          "b" (status));
  while (1) { }
}
