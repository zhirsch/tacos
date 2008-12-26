/*****************************************************************************
 * segments.h
 *****************************************************************************/

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <asm/segments.h>
#include <tacos/types.h>

/* Converts an index and privilege level into a selector into the GDT. */
#define GDT_SELECTOR(x, pl) ((uint16_t)(((x) << 3) | (0 << 2) | ((pl) << 0)))

/*
typedef struct __attribute__ ((packed))
{
   int limit_low  : 16;
   int base_low   : 16;
   int base_mid   : 8;
   int flags1     : 8;
   int limit_high : 4;
   int flags2     : 4;
   int base_high  : 8;
} gdt_entry_t;
*/
typedef uint64_t gdt_entry_t;
typedef uint64_t idt_entry_t;

typedef struct __attribute__ ((packed))
{
   int prev_task_link : 16;
   int reserved1      : 16;

   int esp0           : 32;
   int ss0            : 16;
   int reserved2      : 16;

   int esp1           : 32;
   int ss1            : 16;
   int reserved3      : 16;

   int esp2           : 32;
   int ss2            : 16;
   int reserved4      : 16;

   int cr3            : 32;
   int eip            : 32;
   int eflags         : 32;
   int eax            : 32;
   int ecx            : 32;
   int edx            : 32;
   int ebx            : 32;
   int esp            : 32;
   int ebp            : 32;
   int esi            : 32;
   int edi            : 32;

   int es             : 16;
   int reserved5      : 16;

   int cs             : 16;
   int reserved6      : 16;

   int ss             : 16;
   int reserved7      : 16;
   
   int ds             : 16;
   int reserved8      : 16;
   
   int fs             : 16;
   int reserved9      : 16;

   int gs             : 16;
   int reserved10     : 16;
   
   int ldt_selector   : 16;
   int reserved11     : 16;
   
   int debug_trap     : 1;
   int reserved12     : 15;
   int io_map_base    : 16;
} tss_entry_t;

#endif /* SEGMENTS_H */
