/*****************************************************************************
 * drivers/mmu.c
 *****************************************************************************/

#include <drivers/mmu.h>

#include <tacos/kernel.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/kprintf.h>
#include <tacos/process.h>

static uint8_t mmu_stack[1024];
static void mmu_run(void);

driver_info_t mmu_driver_info = {
   .name        = "mmu",
   .version     = { .major = 0, .minor = 0, .tiny = 1 },
   .entry_point = (uintptr_t)mmu_run,
   .stack       = (uintptr_t)(mmu_stack + sizeof(mmu_stack)),
};

void mmu_run(void)
{
   /*
     perform initial setup

     while true
       get message
       handle message
       send response
   */
}
