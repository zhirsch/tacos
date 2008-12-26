/*****************************************************************************
 * drivers/world.c
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/driver.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/process.h>
#include <tacos/segments.h>

static uint8_t world_stack[1024];
static void world_run(void);

driver_info_t world_driver_info = {
   .name        = "world",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = (uintptr_t)world_run,
   .stack       = (uintptr_t)(world_stack + sizeof(world_stack)),
};

/*****************************************************************************
 * world_run
 *****************************************************************************/
static void world_run(void)
{
   int a;
   Info("World's stack starts near %x.", (uintptr_t)&a);

   while (1) {
      InfoMsg("World");
      process_switch(SLEEPER, RING0);
   }
}
