#ifndef MMU_MMU_H
#define MMU_MMU_H

#include <stdint.h>

#include "multiboot.h"

// The size of each page.
#define PAGESIZE 4096

// Initialize the MMU.  Should only be called once.
void init_mmu(multiboot_info_t* mbi);

enum PageFlags {
	kPagePresent = 1,
	kPageWrite = 2,
	kPageUser = 4,
};

// Map a physical address to an address, with flags.
void mmu_map_page(void* addr, enum PageFlags flags);

// Unmap an address.  Does not free any physical pages.  Returns the
// address of the physical page that was mapped.
void mmu_unmap_page(void* addr);

// Gets the physical address of a page.
uintptr_t mmu_get_paddr(void* addr);

// Gets the flags for the page.
enum PageFlags mmu_get_pde_flags(void* addr);
enum PageFlags mmu_get_pte_flags(void* addr);

// Sets the flags for the page.
void mmu_set_pte_flags(void* addr, enum PageFlags flags);

// Gets the value of the cr3 register.
uintptr_t mmu_get_cr3(void);

static inline void* mmu_round_to_next_page(void* addr) {
	const uintptr_t p = (uintptr_t)addr;
  return (void*)((p & ~(PAGESIZE - 1)) + (p % PAGESIZE > 0) * PAGESIZE);
}

#endif /* MMU_MMU_H */
