#include "screen.h"

unsigned long __stack_chk_guard;

void init_ssp(void) {
  __stack_chk_guard = 0x000a0dff;
}

void __attribute__((noreturn)) __stack_chk_fail(void) {
  puts("stack smashed!\n");
  __asm__ __volatile__ ("cli; hlt");
  while (1);
}
