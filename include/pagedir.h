#ifndef PAGEDIR_H
#define PAGEDIR_H

extern struct pde {
  union {
    struct {
      unsigned int present   :  1;
      unsigned int rw        :  1;
      unsigned int us        :  1;
      unsigned int pwt       :  1;
      unsigned int pcd       :  1;
      unsigned int accessed  :  1;
      unsigned int dirty     :  1;
      unsigned int ps        :  1;
      unsigned int global    :  1;
      unsigned int reserved1 :  3;
      unsigned int pat       :  1;
      unsigned int phys_hi   :  4;
      unsigned int reserved2 :  5;
      unsigned int phys_lo   : 10;
    } __attribute__ ((packed)) page;
    struct {
      unsigned int present   :  1;
      unsigned int rw        :  1;
      unsigned int us        :  1;
      unsigned int pwt       :  1;
      unsigned int pcd       :  1;
      unsigned int accessed  :  1;
      unsigned int reserved1 :  1;
      unsigned int ps        :  1;
      unsigned int reserved2 :  4;
      unsigned int addr      : 20;
    } __attribute__ ((packed)) table;
  } __attribute__ ((packed)) e;
} __attribute__ ((packed)) kernel_pagedir[1024];

struct pte {
  unsigned int present   :  1;
  unsigned int rw        :  1;
  unsigned int us        :  1;
  unsigned int pwt       :  1;
  unsigned int pcd       :  1;
  unsigned int accessed  :  1;
  unsigned int dirty     :  1;
  unsigned int pat       :  1;
  unsigned int global    :  1;
  unsigned int reserved1 :  3;
  unsigned int addr      : 20;
} __attribute__ ((packed));

#endif /* PAGEDIR_H */
