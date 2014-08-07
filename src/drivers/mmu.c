/*****************************************************************************
 * drivers/mmu.c
 *****************************************************************************/

#include <drivers/mmu.h>

#include <tacos/panic.h>
#include <tacos/process.h>

static uint8_t mmu_stack[0x1000];

static void mmu_init(void);
static void mmu_run(void);

driver_info_t mmu_driver_info = {
   .name        = "mmu",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = mmu_run,
   .init        = mmu_init,
   .stack       = mmu_stack + sizeof(mmu_stack),
};

void mmu_init(void)
{
   Info0("Initializing the mmu driver.");
}

void mmu_run(void)
{
   Info0("Inside the MMU driver.");
   Process_Switch(SYSTEM, RING0);
   Panic0("Inside the MMU driver.");

   /*
     perform initial setup

     while true
       get message
       handle message
       send response
   */
}
