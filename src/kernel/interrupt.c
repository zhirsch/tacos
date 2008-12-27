#include <tacos/kernel.h>
#include <tacos/interrupt.h>
#include <tacos/task.h>
#include <tacos/gdt.h>

/* An entry in the IDT that describes an interrupt's task gate. */
typedef struct __attribute__((__packed__))
{
   unsigned int reserved1 : 16;
   unsigned int selector  : 16;
   unsigned int reserved2 : 8;
   unsigned int type      : 5;
   unsigned int dpl       : 2;
   unsigned int present   : 1;
   unsigned int reserved4 : 16;
} idt_task_gate_t;

/* An entry in the IDT. */
typedef uint64_t idt_entry_t;

extern uint8_t kstktop[];
extern idt_entry_t idt[];

/*****************************************************************************
 * Interrupt_AssignNewHandler
 *   Assign a new handler for a specific interrupt. Any existing handler for
 *   the interrupt is removed.
 *****************************************************************************/
void Interrupt_AssignNewHandler(uint8_t intr, interrupt_handler_t hldr)
{
   gdt_task_entry_t entry;
   idt_task_gate_t gate;
   task_state_t *state;

   /* Create a new entry in the GDT for this interrupt. */
   state = TASK_GetInterruptTaskState(intr);
   entry = Gdt_CreateTaskEntry(state);
   GDT_InsertInterrupt(intr, entry);

   /* Create this interrupt's task in the TSS. */
   state->esp0   = (uintptr_t)kstktop;
   state->ss0    = GDT_CreateSelector(2, RING0);
   state->cr3    = memory_get_cr3();
   state->eip    = (uintptr_t)hldr;
   state->eflags = 0x0002;
   state->esp    = (uintptr_t)kstktop;
   state->cs     = GDT_CreateSelector(1, RING0);
   state->ss     = GDT_CreateSelector(2, RING0);
   state->ds     = GDT_CreateSelector(2, RING0);
   state->es     = GDT_CreateSelector(2, RING0);
   state->fs     = GDT_CreateSelector(2, RING0);
   state->gs     = GDT_CreateSelector(2, RING0);

   /* Create the interrupt's task gate. */
   gate.reserved1 = 0;
   gate.reserved1 = 0;
   gate.selector  = INTERRUPT_CreateSelector(intr, RING0);
   gate.reserved2 = 0;
   gate.type      = 0x5;
   gate.dpl       = 0x0;
   gate.present   = 0x1;
   gate.reserved4 = 0x0;

   /* Add the task gate to the IDT. */
   idt[intr] = *((uint64_t *)&gate);
}
