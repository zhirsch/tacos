#ifndef DT_H
#define DT_H

#include <stdint.h>

// The global descriptor table.
extern struct gdt {
  unsigned int limit_lo    : 16;
  unsigned int base_lo     : 24;
  unsigned int type        :  4;
  unsigned int reserved1   :  1;
  unsigned int dpl         :  2;
  unsigned int present     :  1;
  unsigned int limit_hi    :  4;
  unsigned int available   :  1;
  unsigned int reserved2   :  2;
  unsigned int granularity :  1;
  unsigned int base_hi     :  8;
} __attribute__ ((packed)) gdt[];

// The interrupt descriptor table.
extern struct __attribute__ ((packed)) {
  unsigned int offset_lo : 16;
  unsigned int selector  : 16;
  unsigned int reserved  :  8;
  unsigned int type      :  5;
  unsigned int dpl       :  2;
  unsigned int present   :  1;
  unsigned int offset_hi : 16;
} idt[];

#endif /* DT_H */
