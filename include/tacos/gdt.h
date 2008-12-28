/*****************************************************************************
 * gdt.h
 *****************************************************************************/

#ifndef SEGMENT_H
#define SEGMENT_H

#include <asm/gdt.h>
#include <tacos/types.h>
#include <tacos/task.h>

typedef uint16_t gdt_selector_t;

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
} gdt_task_entry_t;

/* Converts an index and privilege level into a selector into the GDT. */
#define GDT_CreateSelector(x, pl) \
   ((uint16_t)(((x) << 3) | (0 << 2) | ((pl) << 0)))

/* Insert a process into the GDT. */
#define GDT_InsertProcess(pid, entry) \
   Gdt_InsertTask(GDT_TASK_BASE + (pid), (entry))

/* Insert an interrupt into the GDT. */
#define GDT_InsertInterrupt(intr, entry) \
   Gdt_InsertTask(GDT_TASK_BASE + NUM_PROCESSES + (intr), (entry))

/**
 * Gdt_InsertTask
 *
 * Add a task to the GDT.
 */
extern void Gdt_InsertTask(uint16_t index, gdt_task_entry_t entry);

/**
 * Gdt_CreateTaskEntry
 *
 * Create a gdt_task_entry_t suitable for insertion into the GDT.
 */
extern gdt_task_entry_t Gdt_CreateTaskEntry(task_state_t *base);

/**
 * Gdt_CreateTaskEntryAndInsert
 *
 * Create a GDT task entry and insert into the GDT at the given index.
 */
extern void Gdt_CreateTaskEntryAndInsert(uint16_t index, task_state_t *base);

/**
 * Gdt_EntryFromSelector
 *
 * Return the gdt_task_entry_t pointed to by the given selector.
 */
extern gdt_task_entry_t *Gdt_EntryFromSelector(gdt_selector_t selector);

#endif /* SEGMENT_H */
