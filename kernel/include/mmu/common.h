#ifndef MMU_COMMON_H
#define MMU_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"

// Initializes the MMU.  Should only be called once.
void init_mmu(multiboot_info_t* mbi);

#define MMU_PAGE_PRESENT 1
#define MMU_PAGE_WRITE   2
#define MMU_PAGE_USER    4

// The size of each page.
#define PAGESIZE 4096

static inline uintptr_t mmu_round_to_next_page(uintptr_t addr) {
  return (addr & ~(PAGESIZE - 1)) + (addr % PAGESIZE > 0) * PAGESIZE;
}

// Maps a linear address (which must be page aligned) to a physical page with
// the given flags.
void mmu_map_page(void* laddr, uint8_t flags);

// Unmaps a linear address.  If the underlying page is no longer used, frees it.
void mmu_unmap_page(void* laddr);

#endif /* MMU_COMMON_H */
