#ifndef MMU_LINEAR_H
#define MMU_LINEAR_H

#include <stdint.h>

#include "mmu/common.h"

// Initialize the linear MMU.  Should only be called once.
void init_lmmu();

// Map a physical address to a virtual address, with flags.
// Common flags:
//   0x1 - read-only
//   0x3 - read-write
void lmmu_map_page(uintptr_t paddr, void* laddr, uint8_t flags);

// Unmap a virtual address.  Does not free any physical pages.  Returns the
// address of the physical page that was mapped.
void lmmu_unmap_page(void* laddr);

// Gets the physical address of a linear page.
uintptr_t lmmu_get_paddr(void* laddr);

// Gets the flags for the page.
uint8_t lmmu_get_pde_flags(void* laddr);
uint8_t lmmu_get_pte_flags(void* laddr);

// Sets the flags for the page.
void lmmu_set_pte_flags(void* laddr, uint8_t flags);

// Gets the value of the cr3 register.
uintptr_t lmmu_get_cr3(void);

#endif /* MMU_LINEAR_H */
