/*****************************************************************************
 * drivers/system.c
 *****************************************************************************/

#include <drivers/system.h>
#include <drivers/mmu.h>

#include <tacos/driver.h>
#include <tacos/panic.h>
#include <tacos/cpuid.h>
#include <tacos/kprintf.h>
#include <tacos/process.h>

static uint8_t system_stack[1024];
static void system_run(void);
static void system_announce(void);

extern uint8_t kstktop[];

driver_info_t system_driver_info = {
   .name        = "system",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = system_run,
   .init        = NULL,
   .stack       = system_stack + sizeof(system_stack),
};

/*****************************************************************************
 * system_run
 *****************************************************************************/
static void system_run(void)
{
   system_announce();

   Driver_Load(&mmu_driver_info);

   while (1) {
      Info0("In system process.");
      Process_Yield();
   }
}

/*****************************************************************************
 * system_announce
 *  - Prints out a banner that says some details about the running kernel.
 *****************************************************************************/
static void system_announce(void)
{
   char vendorid[13] = { 0 };
   uint32_t *pvid = (uint32_t *)vendorid;

   cpuid(0x0, pvid + 4, pvid + 0, pvid + 2, pvid + 1);
   kprintf("tacOS version %d.%d.%d, vendorid = %s\n",
      kversion.major, kversion.minor, kversion.tiny, vendorid);

   /* XXX: Figure out why the addresses of pvid change after a call to cpuid */

   pvid = (uint32_t *)vendorid;
   cpuid(0x1, pvid + 0, pvid + 1, pvid + 2, pvid + 3);
   kprintf("  features: edx = %x, ecx = %x\n", pvid[3], pvid[2]);
}
