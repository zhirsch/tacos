#ifndef TSS_H
#define TSS_H

struct gdt;

struct tss {
  unsigned int prev_task_link : 16;
  unsigned int reserved0      : 16;
  unsigned int esp0           : 32;
  unsigned int ss0            : 16;
  unsigned int reserved1      : 16;
  unsigned int esp1           : 32;
  unsigned int ss1            : 16;
  unsigned int reserved2      : 16;
  unsigned int esp2           : 32;
  unsigned int ss2            : 16;
  unsigned int reserved3      : 16;
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
  unsigned int reserved4      : 16;
  unsigned int cs             : 16;
  unsigned int reserved5      : 16;
  unsigned int ss             : 16;
  unsigned int reserved6      : 16;
  unsigned int ds             : 16;
  unsigned int reserved7      : 16;
  unsigned int fs             : 16;
  unsigned int reserved8      : 16;
  unsigned int gs             : 16;
  unsigned int reserved9      : 16;
  unsigned int ldt_selector   : 16;
  unsigned int reserved10     : 16;
  unsigned int debug_trap     :  1;
  unsigned int reserved11     : 15;
  unsigned int io_map_base    : 16;
}  __attribute__ ((packed));

void init_tss(void);

struct tss* get_tss(void);
struct tss* get_prev_tss(void);
struct tss* gdt_to_tss(struct gdt* e);

#endif /* TSS_H */
