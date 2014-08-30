#include "sbrk.h"

#include <stdint.h>

#include "kprintf.h"
#include "mmu.h"
#include "panic.h"

static void assert(int cond);

void sbrk_grow(uintptr_t* cpb, uintptr_t increment) {
  uintptr_t extra = 0;

  if (*cpb & 0x00000FFF) {
    extra = PAGESIZE - (*cpb & 0x00000FFF);
  }

  if (increment <= extra) {
    // The increment is less than or equal to the extra.  Just increment the
    // break.
    *cpb += increment;
    return;
  }

  if (extra > 0) {
    // There's some extra space before the end of the last allocated page.
    // Increment the break by the extra and discount that from the increment.
    *cpb += extra;
    increment -= extra;
    extra = 0;
  }

  assert(extra == 0);
  assert(*cpb % PAGESIZE == 0);

  // The current program break is aligned to a page boundary.  Allocate enough
  // pages to cover the increment.
  while (increment >= PAGESIZE) {
    const uintptr_t paddr = (uintptr_t)mmu_acquire_physical_page();
    kprintf("SBRK: Mapping vaddr %08lx to paddr %08lx\n", *cpb, paddr);
    mmu_map_page((void*)paddr, (void*)*cpb, 0x1 | 0x2 | 0x4);
    *cpb += PAGESIZE;
    increment -= PAGESIZE;
  }
  // Handle any amount that's greater than a multiple of PAGESIZE.
  if (increment > 0) {
    const uintptr_t paddr = (uintptr_t)mmu_acquire_physical_page();
    kprintf("SBRK: Mapping vaddr %08lx to paddr %08lx\n", *cpb, paddr);
    mmu_map_page((void*)paddr, (void*)*cpb, 0x1 | 0x2 | 0x4);
    *cpb += increment;
    increment = 0;
  }

  assert(increment == 0);
}

void sbrk_shrink(uintptr_t* cpb, uintptr_t decrement) {
  uintptr_t extra = 0;

  if (*cpb & 0x00000FFF) {
    extra = *cpb & 0x00000FFF;
  }

  if (decrement <= extra) {
    // The decrement is less than or equal to the extra.  Just decrement the
    // break.
    *cpb -= decrement;
    return;
  }

  if (extra > 0) {
    // A partial page is used.  Decrement the break by the extra and discount
    // that from the decrement.
    *cpb -= extra;
    decrement -= extra;
    extra = 0;
  }

  assert(extra == 0);
  assert(*cpb % PAGESIZE == 0);

  // The current program break is aligned to a page boundary.  Allocate enough
  // pages to cover the increment.
  while (decrement >= PAGESIZE) {
    const uintptr_t paddr = (uintptr_t)mmu_unmap_page((void*)*cpb);
    kprintf("SBRK: Unmapping vaddr %08lx from paddr %08lx\n", *cpb, paddr);
    mmu_release_physical_page((void*)paddr);
    *cpb -= PAGESIZE;
    decrement -= PAGESIZE;
  }
  // Handle any amount that's greater than a multiple of PAGESIZE.
  if (decrement > 0) {
    *cpb -= decrement;
    kprintf("SBRK: Shrinking break to partial page at vaddr %08lx\n", *cpb);
    decrement = 0;
  }

  assert(decrement == 0);
}

static void assert(int cond) {
  if (!cond) {
    panic("SBRK: assert failed\n");
  }
}
