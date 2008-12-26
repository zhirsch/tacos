/*****************************************************************************
 * segments.h
 *****************************************************************************/

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <asm/segments.h>
#include <tacos/types.h>

/* Converts an index and privilege level into a selector into the GDT. */
#define GDT_SELECTOR(x, pl) ((uint16_t)(((x) << 3) | (0 << 2) | ((pl) << 0)))

/* Insert a process TSS into the GDT. */
void gdt_insert_process(uint16_t pid, uint64_t entry);

/* Insert an interrupt TSS into the GDT. */
void gdt_insert_interrupt(uint16_t intr, uint64_t entry);

/* Create a TSS entry, suitable for insertion into the GDT. */
uint64_t gdt_create_tss_entry(uintptr_t base);

typedef struct __attribute__ ((packed))
{
   unsigned int prev_task_link : 16;
   unsigned int reserved1      : 16;

   unsigned int esp0           : 32;
   unsigned int ss0            : 16;
   unsigned int reserved2      : 16;

   unsigned int esp1           : 32;
   unsigned int ss1            : 16;
   unsigned int reserved3      : 16;

   unsigned int esp2           : 32;
   unsigned int ss2            : 16;
   unsigned int reserved4      : 16;

   unsigned int cr3            : 32;
   unsigned int eip            : 32;
   unsigned int eflags         : 32;
   unsigned int eax            : 32;
   unsigned int ecx            : 32;
   unsigned int edx            : 32;
   unsigned int ebx            : 32;
   unsigned int esp            : 32;
   unsigned int ebp            : 32;
   unsigned int esi            : 32;
   unsigned int edi            : 32;

   unsigned int es             : 16;
   unsigned int reserved5      : 16;

   unsigned int cs             : 16;
   unsigned int reserved6      : 16;

   unsigned int ss             : 16;
   unsigned int reserved7      : 16;

   unsigned int ds             : 16;
   unsigned int reserved8      : 16;

   unsigned int fs             : 16;
   unsigned int reserved9      : 16;

   unsigned int gs             : 16;
   unsigned int reserved10     : 16;

   unsigned int ldt_selector   : 16;
   unsigned int reserved11     : 16;

   unsigned int debug_trap     : 1;
   unsigned int reserved12     : 15;
   unsigned int io_map_base    : 16;
} tss_entry_t;

tss_entry_t tss_get_current(void);
tss_entry_t tss_get_previous(void);

#endif /* SEGMENTS_H */
