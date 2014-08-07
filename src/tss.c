#include "tss.h"

#include "dt.h"

struct tss* get_tss(void) {
  int tr;
  __asm__ __volatile__ ("str %0" : "=r" (tr));
  return gdt_to_tss(get_gdt_entry(tr));
}

struct tss* get_prev_tss(void) {
  const struct tss* tss = get_tss();
  return gdt_to_tss(get_gdt_entry(tss->prev_task_link));
}

struct tss* gdt_to_tss(struct gdt* e) {
  return (struct tss*)(e->base_lo | (e->base_hi) << 24);
}
