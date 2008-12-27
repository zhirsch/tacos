#include <tacos/gdt.h>

extern gdt_task_entry_t gdt[];

/**
 * Gdt_InsertTask
 *
 * Add a task to the GDT.
 */
void Gdt_InsertTask(uint16_t index, gdt_task_entry_t entry)
{
   gdt[index] = entry;
}


/**
 * Gdt_CreateTaskEntry
 *
 * Create a gdt_task_entry_t suitable for insertion into the GDT.
 */
gdt_task_entry_t Gdt_CreateTaskEntry(task_state_t *base)
{
   /* Fill in the GDT with segments for each of the TSSs. */
   gdt_task_entry_t entry = {
      .limit_lo    = 0x67, /* 103 == (minimum size of a TSS) - 1 */
      .base_lo     = ((uintptr_t)base & 0x0000ffff) >> 0,
      .base_mid    = ((uintptr_t)base & 0x00ff0000) >> 16,
      .type        = 0x9,  /* 1001 -- the type of a TSS */
      .reserved1   = 0x0,  /* must be zero */
      .dpl         = 0x0,  /* data protection layer -- RING0 */
      .present     = 0x1,  /* present (can be switched to) */
      .limit_hi    = 0x0,
      .available   = 0x0,
      .reserved2   = 0x0,  /* must be zero */
      .granularity = 0x0,  /* the limit is in bytes */
      .base_hi     = ((uintptr_t)base & 0xff000000) >> 24,
   };
   return entry;
}


/**
 * Gdt_CreateTaskEntryAndInsert
 *
 * Create a GDT task entry and insert into the GDT at the given index.
 */
void Gdt_CreateTaskEntryAndInsert(uint16_t index, task_state_t *base)
{
   gdt_task_entry_t entry = Gdt_CreateTaskEntry(base);
   Gdt_InsertTask(index, entry);
}


/**
 * Gdt_EntryFromSelector
 *
 * Return the gdt_task_entry_t pointed to by the given selector.
 */
gdt_task_entry_t *Gdt_EntryFromSelector(gdt_selector_t selector)
{
   return (gdt_task_entry_t *)(gdt + (selector >> 3));
}

