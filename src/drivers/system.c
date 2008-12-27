/*****************************************************************************
 * drivers/system.c
 *****************************************************************************/

#include <drivers/system.h>
#include <drivers/mmu.h>
#include <drivers/hello.h>
#include <drivers/world.h>
#include <drivers/sleeper.h>

#include <tacos/kernel.h>
#include <tacos/driver.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/cpuid.h>
#include <tacos/kprintf.h>
#include <tacos/process.h>
#include <tacos/gdt.h>

#include <lib/datetime.h>

static uint8_t system_stack[1024];
static void system_run(void);
static void system_announce(void);
static void system_load_driver(driver_info_t *driverinfo, pid_t pid, int pl);

extern uint8_t kstktop[];

driver_info_t system_driver_info = {
   .name             = "system",
   .version          = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point_func = system_run,
   .init_func        = NULL,
   .stack            = system_stack + sizeof(system_stack),
};

/*****************************************************************************
 * system_run
 *****************************************************************************/
static void system_run(void)
{
   system_announce();

//   system_load_driver(&mmu_driver_info, MMU, RING0);
   system_load_driver(&hello_driver_info, HELLO, RING0);
   system_load_driver(&world_driver_info, WORLD, RING0);
   system_load_driver(&sleeper_driver_info, SLEEPER, RING0);

   while (1) {
      InfoMsg("In system process.");
      Process_Switch(HELLO, RING0);
      sleep(3);
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

/*****************************************************************************
 * system_load_driver
 *****************************************************************************/
static void system_load_driver(driver_info_t *driverinfo, pid_t pid, int pl)
{
   task_state_t *state = TASK_GetProcessTaskState(pid);

   state->esp0 = (uintptr_t)kstktop;
   state->ss0  = GDT_CreateSelector(2, RING0);
   state->cr3  = memory_get_cr3();
   state->eip  = (uintptr_t)driverinfo->entry_point_func;
   state->esp  = (uintptr_t)driverinfo->stack;
   state->cs   = GDT_CreateSelector(1, pl);
   state->ss   = GDT_CreateSelector(2, pl);
   state->ds   = GDT_CreateSelector(2, pl);
   state->es   = GDT_CreateSelector(2, pl);
   state->fs   = GDT_CreateSelector(2, pl);
   state->gs   = GDT_CreateSelector(2, pl);

   Info("Loaded %s driver, version %d.%d.%d",
	driverinfo->name,
	driverinfo->version.major,
	driverinfo->version.minor,
	driverinfo->version.tiny);
}
