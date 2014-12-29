#include "mmu/sbrk.h"

#include <stdint.h>

#include "log.h"
#include "mmu/common.h"

#define LOG(...) log("SBRK", __VA_ARGS__)
#define PANIC(...) panic("SBRK", __VA_ARGS__)

static uintptr_t current_kernel_program_break = (uintptr_t)LDSYM_LADDR(kernel_heap_start);
// TODO: Detect when the kernel is out of heap space.

static void assert(int cond);

void mmu_sbrk_grow(uintptr_t* cpb, uintptr_t increment) {
  uintptr_t extra = 0;
  // TODO: The flags shouldn't include MMU_PAGE_USER if this is for the kernel break.
  // TODO: Use my own kmalloc implementation and there won't be a kernel break.
  const uint8_t flags = MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;

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
    mmu_map_page((void*)*cpb, flags);
    *cpb += PAGESIZE;
    increment -= PAGESIZE;
  }
  // Handle any amount that's greater than a multiple of PAGESIZE.
  if (increment > 0) {
    mmu_map_page((void*)*cpb, flags);
    *cpb += increment;
    increment = 0;
  }

  assert(increment == 0);
}

void mmu_sbrk_shrink(uintptr_t* cpb, uintptr_t decrement) {
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

  // The current program break is aligned to a page boundary.  Free enough pages
  // to satisfy the decrement.
  while (decrement >= PAGESIZE) {
    mmu_unmap_page((void*)*cpb);
    *cpb -= PAGESIZE;
    decrement -= PAGESIZE;
  }
  // Handle any amount that's greater than a multiple of PAGESIZE.
  if (decrement > 0) {
    *cpb -= decrement;
    decrement = 0;
  }

  assert(decrement == 0);
}

void* mmu_kmorecore(intptr_t increment) {
  const uintptr_t vaddr = current_kernel_program_break;
  // Make sure that a multiple of a page was requested.
  if (increment & (PAGESIZE - 1)) {
    PANIC("ksbrk(%lx) is not page aligned\n", increment);
  } else if (increment < 0) {
    mmu_sbrk_shrink(&current_kernel_program_break, -increment);
  } else if (increment > 0) {
    mmu_sbrk_grow(&current_kernel_program_break, increment);
  }
  return (void*)vaddr;
}

static void assert(int cond) {
  if (!cond) {
    PANIC("assert failed\n");
  }
}
