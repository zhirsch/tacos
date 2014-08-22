#ifndef MMU_H
#define MMU_H

#include "multiboot.h"

// Initialize the MMU.  Should only be called once.
void init_mmu(multiboot_info_t* mbi);

// Allocated a new physical page.
void* mmu_new_physical_page(void);

// Map a physical address to a virtual address, with flags.
// Common flags:
//   0x1 - read-only
//   0x3 - read-write
void mmu_map_page(void* paddr, void* vaddr, int flags);

// Unmap a virtual address.  Does not free any physical pages.
void mmu_unmap_page(void* vaddr);

#endif /* MMU_H */
