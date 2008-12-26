#include <tacos/kernel.h>
#include <tacos/types.h>
#include <tacos/interrupts.h>
#include <tacos/segments.h>

typedef struct __attribute__((__packed__))
{
   unsigned int reserved1 : 16;
   unsigned int selector  : 16;
   unsigned int reserved2 : 8;
   unsigned int type      : 4;
   unsigned int reserved3 : 1;
   unsigned int dpl       : 2;
   unsigned int present   : 1;
   unsigned int reserved4 : 16;
} idt_task_gate_t;

typedef uint64_t idt_entry_t;

extern uint8_t kstktop[];
extern tss_entry_t tss[];
extern idt_entry_t idt[];

void prepare_interrupt_handler(uint8_t intr, intr_handler_t handler)
{
   uint64_t entry = gdt_create_tss_entry((uintptr_t)(tss + NUM_PROCESSES + intr));
   gdt_insert_interrupt(intr, entry);

   tss[NUM_PROCESSES + intr].esp0   = (uintptr_t)kstktop;
   tss[NUM_PROCESSES + intr].ss0    = GDT_SELECTOR(2, RING0);
   tss[NUM_PROCESSES + intr].cr3    = memory_get_cr3();
   tss[NUM_PROCESSES + intr].eip    = (uintptr_t)handler;
   tss[NUM_PROCESSES + intr].eflags = 0x0002;
   tss[NUM_PROCESSES + intr].esp    = (uintptr_t)kstktop;
   tss[NUM_PROCESSES + intr].cs     = GDT_SELECTOR(1, RING0);
   tss[NUM_PROCESSES + intr].ss     = GDT_SELECTOR(2, RING0);
   tss[NUM_PROCESSES + intr].ds     = GDT_SELECTOR(2, RING0);
   tss[NUM_PROCESSES + intr].eax    = 0xdeadbeef;

   idt_task_gate_t gate = {
      .reserved1 = 0,
      .selector  = INTERRUPT_TSS(intr, RING0),
      .reserved2 = 0,
      .type      = 0x5,
      .reserved3 = 0,
      .dpl       = 0x0,
      .present   = 0x1,
      .reserved4 = 0x0,
   };
   idt[intr] = *((uint64_t *)&gate);
}
