/*****************************************************************************
 * kmain.c
 *   The entry point into the kernel. Called from kernel/boot/boot.S.
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/cpuid.h>
#include <tacos/process.h>
#include <tacos/segments.h>
#include <tacos/screen.h>
#include <tacos/kprintf.h>
#include <tacos/interrupts.h>

#include <lib/datetime.h>

#include <drivers/system.h>

#include <multiboot.h>

/* Create a variable to contain the kernel's version. */
version_t kversion = { .major = 0, .minor = 1, .tiny = 1 };

/* Declare some universal data structures. These are defined in boot/boot.S */
extern uint8_t kernel_pagedir[];
extern uint8_t kstktop[];
extern tss_entry_t tss[];

/*****************************************************************************
 * kmain
 *   This is the big salami.
 *****************************************************************************/
void kmain(uint32_t magic, multiboot_info_t *mbi)
{
   /* Make sure the magic number is correct  */
   Assert(magic == MULTIBOOT_BOOTLOADER_MAGIC,
      "Wrong magic number passed to kmain: 0x%x\n", magic);

   /* By the time execution reaches here, the following has occurred:
    *   -- Paging is enabled.
    *   -- The GDT has been created with the following entries:
    *     0: the NULL descriptor
    *     1: a RING0 code segment (for the kernel)
    *     2: a RING0 data segment (for the kernel)
    *     3: a RING3 code segment (for applications)
    *     4: a RING3 data segment (for applications)
    *     5+ space for task segment descriptors for the other processes
    *   -- Space has been allocated in the IDT for 256 entries.
    */

   clearscreen();

   /* Fill in the GDT with segments for each process. */
   for (int i = 0; i < NUM_PROCESSES; i++)
   {
      uint64_t entry = gdt_create_tss_entry((uintptr_t)(tss + i));
      gdt_insert_process(i, entry);
   }

   /* Create the SYSTEM task-state segment. */
   tss[SYSTEM].esp0   = (uintptr_t)kstktop;
   tss[SYSTEM].ss0    = GDT_SELECTOR(2, RING0);
   tss[SYSTEM].cr3    = memory_get_cr3();
   tss[SYSTEM].eip    = system_driver_info.entry_point;
   tss[SYSTEM].eflags = 0x0012;
   tss[SYSTEM].esp    = system_driver_info.stack;
   tss[SYSTEM].cs     = GDT_SELECTOR(1, RING0);
   tss[SYSTEM].ss     = GDT_SELECTOR(2, RING0);
   tss[SYSTEM].ds     = GDT_SELECTOR(2, RING0);
   /* All unset fields are 0 */

   /* Populate the interrupt handlers */
   prepare_interrupt_handler(INTR_DE, intr_divide_error_stub);
   prepare_interrupt_handler(INTR_BP, intr_breakpoint_stub);
   prepare_interrupt_handler(INTR_OF, intr_overflow_stub);
   prepare_interrupt_handler(INTR_BR, intr_bound_range_stub);
   prepare_interrupt_handler(INTR_UD, intr_undefined_opcode_stub);
   prepare_interrupt_handler(INTR_NM, intr_no_math_stub);
   prepare_interrupt_handler(INTR_DF, intr_double_fault_stub);
   prepare_interrupt_handler(INTR_TS, intr_invalid_tss_stub);
   prepare_interrupt_handler(INTR_NP, intr_segment_not_present_stub);
   prepare_interrupt_handler(INTR_GP, intr_general_protection_stub);
   prepare_interrupt_handler(INTR_PF, intr_page_fault_stub);
   prepare_interrupt_handler(INTR_MF, intr_math_fault_stub);
   prepare_interrupt_handler(INTR_AC, intr_alignment_check_stub);
   prepare_interrupt_handler(INTR_MC, intr_machine_check_stub);
   prepare_interrupt_handler(INTR_XF, intr_simd_exception_stub);

   /* Load a temporary, garbage task and switch to the system task */
   __asm__ __volatile__ ("ltr %0" : : "R" PROCESS_TSS(GARBAGE, RING0));
   __asm__ __volatile__ ("sti");

   process_switch(SYSTEM, RING0);
   Panic("%s", "Why am I here?");
}
