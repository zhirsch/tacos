#ifndef TSS_H
#define TSS_H

#include "mmu/common.h"

struct tss {
  unsigned int prev_task_link : 16;  // 0x00
  unsigned int reserved0      : 16;
  unsigned int esp0           : 32;  // 0x04
  unsigned int ss0            : 16;  // 0x08
  unsigned int reserved1      : 16;
  unsigned int esp1           : 32;  // 0x0c
  unsigned int ss1            : 16;  // 0x10
  unsigned int reserved2      : 16;
  unsigned int esp2           : 32;  // 0x14
  unsigned int ss2            : 16;  // 0x18
  unsigned int reserved3      : 16;
  unsigned int cr3            : 32;  // 0x1c
  unsigned int eip            : 32;  // 0x20
  unsigned int eflags         : 32;  // 0x24
  unsigned int eax            : 32;  // 0x28
  unsigned int ecx            : 32;  // 0x2c
  unsigned int edx            : 32;  // 0x30
  unsigned int ebx            : 32;  // 0x34
  unsigned int esp            : 32;  // 0x38
  unsigned int ebp            : 32;  // 0x3c
  unsigned int esi            : 32;  // 0x40
  unsigned int edi            : 32;  // 0x44
  unsigned int es             : 16;  // 0x48
  unsigned int reserved4      : 16;
  unsigned int cs             : 16;  // 0x4c
  unsigned int reserved5      : 16;
  unsigned int ss             : 16;  // 0x50
  unsigned int reserved6      : 16;
  unsigned int ds             : 16;  // 0x54
  unsigned int reserved7      : 16;
  unsigned int fs             : 16;  // 0x58
  unsigned int reserved8      : 16;
  unsigned int gs             : 16;  // 0x5c
  unsigned int reserved9      : 16;
  unsigned int ldt_selector   : 16;  // 0x60
  unsigned int reserved10     : 16;
  unsigned int debug_trap     :  1;  // 0x64
  unsigned int reserved11     : 15;
  unsigned int io_map_base    : 16;  // 0x66
}  __attribute__ ((packed));

extern struct tss g_tss __attribute__ ((aligned(PAGESIZE)));

#endif /* TSS_H */
