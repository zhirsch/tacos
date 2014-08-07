/* Support for GCC's stack smash protection (-fstack-protector-all). */

#include <tacos/ssp.h>

#include <tacos/panic.h>

unsigned long __stack_chk_guard;

void init_ssp(void)
{
  __stack_chk_guard = 0x000a0dff;
}

void __attribute__((noreturn)) __stack_chk_fail(void)
{
  Panic0("Stack is smashed!");
}
