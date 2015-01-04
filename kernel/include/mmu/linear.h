#ifndef MMU_LINEAR_H
#define MMU_LINEAR_H

#include <stdint.h>

#include "mmu/common.h"

static inline uintptr_t mmu_round_to_next_page(uintptr_t addr) {
  return (addr & ~(PAGESIZE - 1)) + (addr % PAGESIZE > 0) * PAGESIZE;
}

// Initialize the linear MMU.  Should only be called once.
void init_lmmu();

// Map a physical address to a virtual address, with flags.
// Common flags:
//   0x1 - read-only
//   0x3 - read-write
void lmmu_map_page(uintptr_t paddr, void* laddr, uint8_t flags);

// Unmap a virtual address.  Does not free any physical pages.  Returns the
// address of the physical page that was mapped.
uintptr_t lmmu_unmap_page(void* laddr);

// Sets the flags for the page.
void lmmu_set_page_flags(void* laddr, uint8_t flags);

// Gets the value of the cr3 register.
uintptr_t lmmu_get_cr3(void);

// Unmaps all pages in the current page directory, except for the kernel.
void lmmu_reset_cr3(void);

#endif /* MMU_LINEAR_H */
