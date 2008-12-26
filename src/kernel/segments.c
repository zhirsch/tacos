#include <tacos/kernel.h>
#include <tacos/types.h>
#include <tacos/segments.h>
#include <tacos/process.h>
#include <tacos/kprintf.h>

typedef struct __attribute__((__packed__))
{
   unsigned int limit_lo    : 16;
   unsigned int base_lo     : 16;
   unsigned int base_mid    : 8;
   unsigned int type        : 4;
   unsigned int reserved1   : 1;
   unsigned int dpl         : 2;
   unsigned int present     : 1;
   unsigned int limit_hi    : 4;
   unsigned int available   : 1;
   unsigned int reserved2   : 2;
   unsigned int granularity : 1;
   unsigned int base_hi     : 8;
} gdt_tss_t;

typedef uint64_t gdt_entry_t;

extern gdt_entry_t gdt[];
extern tss_entry_t tss[];
static void _gdt_insert(uint16_t index, uint64_t entry);

/* Insert a process TSS into the GDT. */
void gdt_insert_process(uint16_t pid, uint64_t entry)
{
   _gdt_insert(GDT_TSS_BASE + pid, entry);
}

/* Insert an interrupt TSS into the GDT. */
void gdt_insert_interrupt(uint16_t intr, uint64_t entry)
{
   _gdt_insert(GDT_TSS_BASE + NUM_PROCESSES + intr, entry);
}

/* Create a TSS entry, suitable for insertion into the GDT. */
uint64_t gdt_create_tss_entry(uintptr_t base)
{
  /* Fill in the GDT with segments for each of the TSSs. */
   gdt_tss_t entry = {
      .limit_lo    = 0x67, /* 103 == (minimum size of a TSS) - 1 */
      .base_lo     = (base & 0x0000ffff) >> 0,
      .base_mid    = (base & 0x00ff0000) >> 16,
      .type        = 0x9,  /* 1001 -- the type of a TSS */
      .reserved1   = 0x0,  /* must be zero */
      .dpl         = 0x0,  /* data protection layer -- RING0 */
      .present     = 0x1,  /* present (can be switched to) */
      .limit_hi    = 0x0,
      .available   = 0x0,
      .reserved2   = 0x0,  /* must be zero */
      .granularity = 0x0,  /* the limit is in bytes */
      .base_hi     = (base & 0xff000000) >> 24,
   };
   return *((uint64_t *)&entry);
}

tss_entry_t tss_get_current(void)
{
   uint16_t selector;
   gdt_tss_t entry;
   uintptr_t addr;

   __asm__ __volatile__ ("str %0" : "=R" (selector));

   entry = *((gdt_tss_t *)(gdt + (selector >> 3)));
   addr = (entry.base_hi << 24) | (entry.base_mid << 16) | entry.base_lo;
   return *((tss_entry_t *)addr);
}

tss_entry_t tss_get_previous(void)
{
   uint16_t selector;
   gdt_tss_t entry;
   uintptr_t addr;

   selector = tss_get_current().prev_task_link;

   entry = *((gdt_tss_t *)(gdt + (selector >> 3)));
   addr = (entry.base_hi << 24) | (entry.base_mid << 16) | entry.base_lo;
   return *((tss_entry_t *)addr);
}

static void _gdt_insert(uint16_t index, uint64_t entry)
{
   gdt[index] = entry;
}
