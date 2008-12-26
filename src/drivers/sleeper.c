/*****************************************************************************
 * drivers/sleeper.c
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/driver.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/process.h>
#include <tacos/segments.h>
#include <lib/datetime.h>

static uint8_t sleeper_stack[1024];
static void sleeper_run(void);

driver_info_t sleeper_driver_info = {
   .name        = "sleeper",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = (uintptr_t)sleeper_run,
   .stack       = (uintptr_t)(sleeper_stack + sizeof(sleeper_stack)),
};

/*****************************************************************************
 * sleeper_run
 *****************************************************************************/
static void sleeper_run(void)
{
   int a;
   Info("Sleeper's stack starts near %x.", (uintptr_t)&a);

   while (1) {
      sleep(3);
      process_switch(HELLO, RING0);
   }
}
