#include "log.h"

#define PANIC(...) panic("SSP", __VA_ARGS__)

unsigned long __stack_chk_guard;

void init_ssp(void) {
  __stack_chk_guard = 0x000a0dff;
}

void __attribute__((noreturn)) __stack_chk_fail(void) {
  PANIC("stack smashed!\n");
}
