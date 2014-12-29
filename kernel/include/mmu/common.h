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
void mmu_map_page(void* laddr, uint8_t flags);

// Unmaps a linear address.  If the underlying page is no longer used, frees it.
void mmu_unmap_page(void* laddr);

// Sets the flags for a linear address (which must be page aligned) that is
// already mapped.
void mmu_set_page_flags(void* laddr, uint8_t flags);

// Convinience functions for calling mmu_map_page with the right flags.
void mmu_map_system_rw_page(void* laddr);
void mmu_map_user_rw_page(void* laddr);

// The size of each page.
#define PAGESIZE 4096

typedef struct LAddr* LAddr;
typedef struct PAddr* PAddr;

#define LDSYM_LADDR(x) ((LAddr)(&(LDSYM_laddr_ ## x)))
#define LDSYM_PADDR(x) ((PAddr)(&(LDSYM_paddr_ ## x)))
#define LADDR_TO_PADDR(x) ((PAddr)((uintptr_t)(x) - (uintptr_t)LDSYM_LADDR(origin)))

// Symbols that mark parts of the kernel's linear address space.
extern LAddr LDSYM_laddr_kernel_heap_end;
extern LAddr LDSYM_laddr_kernel_heap_start;
extern LAddr LDSYM_laddr_kernel_pmmu_page_stack_bottom;
extern LAddr LDSYM_laddr_kernel_stack_bottom;
extern LAddr LDSYM_laddr_kernel_stack_bottom_fence;
extern LAddr LDSYM_laddr_kernel_stack_top;
extern LAddr LDSYM_laddr_kernel_stack_top_fence;
extern LAddr LDSYM_laddr_kernel_start;
extern LAddr LDSYM_laddr_origin;

// Symbols that mark parts of the kernel's physical address space.
extern PAddr LDSYM_paddr_kernel_end;
extern PAddr LDSYM_paddr_kernel_pagedir;
extern PAddr LDSYM_paddr_kernel_ro_end;
extern PAddr LDSYM_paddr_kernel_ro_start;
extern PAddr LDSYM_paddr_kernel_rw_end;
extern PAddr LDSYM_paddr_kernel_rw_start;
extern PAddr LDSYM_paddr_kernel_stack_bottom;
extern PAddr LDSYM_paddr_kernel_stack_bottom_fence;
extern PAddr LDSYM_paddr_kernel_stack_top;
extern PAddr LDSYM_paddr_kernel_stack_top_fence;
extern PAddr LDSYM_paddr_kernel_start;

static inline bool paddr_within(PAddr lhs, PAddr value, PAddr rhs) {
  return ((uintptr_t)lhs <= (uintptr_t)value && (uintptr_t)value < (uintptr_t)rhs);
}

static inline bool laddr_within(LAddr lhs, LAddr value, LAddr rhs) {
  return ((uintptr_t)lhs <= (uintptr_t)value && (uintptr_t)value < (uintptr_t)rhs);
}

#endif /* MMU_COMMON_H */
