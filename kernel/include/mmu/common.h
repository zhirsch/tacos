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

// Maps a linear address (which must be page aligned) to a physical page with
// the given flags.
uintptr_t mmu_map_page(void* laddr, uint8_t flags);

// Unmaps a linear address.  If the underlying page is no longer used, frees it.
void mmu_unmap_page(void* laddr);

// Sets the flags for a linear address (which must be page aligned) that is
// already mapped.
void mmu_set_page_flags(void* laddr, uint8_t flags);

// Convinience functions for calling mmu_map_page with the right flags.
uintptr_t mmu_map_system_rw_page(void* laddr);
void mmu_map_user_rw_page(void* laddr);

// Makes a copy of the current address space.
uintptr_t mmu_clone_address_space(void);

// Frees all the pages in an address space.
void mmu_free_address_space(uintptr_t cr3);

// The size of each page.
#define PAGESIZE 4096

#endif /* MMU_COMMON_H */
