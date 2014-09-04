#ifndef MMU_H
#define MMU_H

#include <stdint.h>

#include "multiboot.h"

// The size of each page.
#define PAGESIZE 4096

#define SEGMENT_KERNEL_CODE ((1 << 3) | 0x0)
#define SEGMENT_KERNEL_DATA ((2 << 3) | 0x0)
#define SEGMENT_USER_CODE   ((3 << 3) | 0x3)
#define SEGMENT_USER_DATA   ((4 << 3) | 0x3)

static inline uintptr_t mmu_round_to_next_page(uintptr_t addr) {
  return (addr & ~(PAGESIZE - 1)) + (addr % PAGESIZE > 0) * PAGESIZE;
}

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

// Sets the flags for the page.
void mmu_set_page_flags(void* vaddr, int flags);

// Gets the value of the cr3 register.
uintptr_t mmu_get_cr3(void);

// Unmaps all pages in the current page directory, except for the kernel.
void mmu_reset_cr3(void);

// Kernel's implemnetation of sbrk.
void* ksbrk(intptr_t increment);

#endif /* MMU_H */
