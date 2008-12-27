/*****************************************************************************
 * kmain.c
 *   The entry point into the kernel. Called from kernel/boot/boot.S.
 *****************************************************************************/

#include <tacos/kernel.h>
#include <tacos/types.h>
#include <tacos/panic.h>
#include <tacos/process.h>
#include <tacos/task.h>
#include <tacos/gdt.h>
#include <tacos/interrupt.h>
#include <tacos/screen.h>

#include <drivers/system.h>

#include <multiboot.h>

/* Create a variable to contain the kernel's version. */
version_t kversion = { .major = 0, .minor = 1, .tiny = 1 };

/* Declare some universal data structures. These are defined in boot/boot.S */
extern uint8_t kstktop[];

/*****************************************************************************
 * kmain
 *   This is the big salami.
 *****************************************************************************/
void kmain(uint32_t magic, multiboot_info_t *mbi)
{
   task_state_t *state;

   /* Make sure the magic number is correct  */
   Assert(magic == MULTIBOOT_BOOTLOADER_MAGIC,
      "Wrong magic number passed to kmain: 0x%x\n", magic);

   clearscreen();

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

   /* Fill in the GDT with segments for each process. */
   for (int i = 0; i < NUM_PROCESSES; i++)
   {
      gdt_task_entry_t entry;

      state = TASK_GetProcessTaskState(i);
      entry = Gdt_CreateTaskEntry(state);
      GDT_InsertProcess(i, entry);
   }

   /* Create the SYSTEM task-state segment. */
   state = TASK_GetProcessTaskState(SYSTEM);
   state->esp0   = (uintptr_t)kstktop;
   state->ss0    = GDT_CreateSelector(2, RING0);
   state->cr3    = memory_get_cr3();
   state->eip    = (uintptr_t)system_driver_info.entry_point_func;
   state->eflags = 0x0012;
   state->esp    = (uintptr_t)system_driver_info.stack;
   state->cs     = GDT_CreateSelector(1, RING0);
   state->ss     = GDT_CreateSelector(2, RING0);
   state->ds     = GDT_CreateSelector(2, RING0);
   state->es     = GDT_CreateSelector(2, RING0);
   state->fs     = GDT_CreateSelector(2, RING0);
   state->gs     = GDT_CreateSelector(2, RING0);

   /* Populate the interrupt handlers */
   Interrupt_AssignNewHandler(INTR_DE, intr_divide_error_stub);
   Interrupt_AssignNewHandler(INTR_BP, intr_breakpoint_stub);
   Interrupt_AssignNewHandler(INTR_OF, intr_overflow_stub);
   Interrupt_AssignNewHandler(INTR_BR, intr_bound_range_stub);
   Interrupt_AssignNewHandler(INTR_UD, intr_undefined_opcode_stub);
   Interrupt_AssignNewHandler(INTR_NM, intr_no_math_stub);
   Interrupt_AssignNewHandler(INTR_DF, intr_double_fault_stub);
   Interrupt_AssignNewHandler(INTR_TS, intr_invalid_tss_stub);
   Interrupt_AssignNewHandler(INTR_NP, intr_segment_not_present_stub);
   Interrupt_AssignNewHandler(INTR_GP, intr_general_protection_stub);
   Interrupt_AssignNewHandler(INTR_PF, intr_page_fault_stub);
   Interrupt_AssignNewHandler(INTR_MF, intr_math_fault_stub);
   Interrupt_AssignNewHandler(INTR_AC, intr_alignment_check_stub);
   Interrupt_AssignNewHandler(INTR_MC, intr_machine_check_stub);
   Interrupt_AssignNewHandler(INTR_XF, intr_simd_exception_stub);

   /* Load a temporary, garbage task and switch to the system task */
   Task_SetTaskRegister(PROCESS_CreateSelector(GARBAGE, RING0));
   Process_Switch(SYSTEM, RING0);

   /* The system task should never terminate. */
   Panic("%s", "Why am I here?");
}
