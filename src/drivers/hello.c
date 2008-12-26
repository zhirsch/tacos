/*****************************************************************************
 * drivers/hello.c
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/driver.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/process.h>
#include <tacos/segments.h>

static uint8_t hello_stack[1024];
static void hello_run(void);

driver_info_t hello_driver_info = {
   .name        = "hello",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = (uintptr_t)hello_run,
   .stack       = (uintptr_t)(hello_stack + sizeof(hello_stack)),
};

/*****************************************************************************
 * hello_run
 *****************************************************************************/
static void hello_run(void)
{
   int a;
   Info("Hello's stack starts near %x.", (uintptr_t)&a);

   while (1) {
      InfoMsg("Hello");
      process_switch(WORLD, RING0);
   }
}
