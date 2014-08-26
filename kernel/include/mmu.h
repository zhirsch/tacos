#ifndef MMU_H
#define MMU_H

#include <stdint.h>

#include "multiboot.h"

// The size of each page.
#define PAGESIZE 4096

// Initialize the MMU.  Should only be called once.
void init_mmu(multiboot_info_t* mbi);

// Acquire a physical page.
void* mmu_acquire_physical_page(void);

// Release a physical page.
void mmu_release_physical_page(void* paddr);

// Gets the physical address backing the vaddr.
void* mmu_get_paddr(const void* vaddr);

// Map a physical address to a virtual address, with flags.
// Common flags:
//   0x1 - read-only
//   0x3 - read-write
void mmu_map_page(void* paddr, void* vaddr, int flags);

// Unmap a virtual address.  Does not free any physical pages.  Returns the
// address of the physical page that was mapped.
void* mmu_unmap_page(void* vaddr);

// Kernel's implemnetation of sbrk.
void* ksbrk(intptr_t increment);

#endif /* MMU_H */
