/*****************************************************************************
 * drivers/world.c
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/driver.h>
#include <tacos/panic.h>
#include <tacos/process.h>

static uint8_t world_stack[1024];
static void world_run(void);

driver_info_t world_driver_info = {
   .name             = "world",
   .version          = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point_func = world_run,
   .init_func        = NULL,
   .stack            = world_stack + sizeof(world_stack),
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
      Process_Switch(SLEEPER, RING0);
   }
}
