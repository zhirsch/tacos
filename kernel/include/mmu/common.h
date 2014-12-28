#ifndef MMU_COMMON_H
#define MMU_COMMON_H

#include <stdbool.h>
#include <stddef.h>

// The size of each page.
#define PAGESIZE 4096

typedef struct LAddr* LAddr;
typedef struct PAddr* PAddr;

#define LDSYM_LADDR(x) ((LAddr)(&(LDSYM_laddr_ ## x)))
#define LDSYM_PADDR(x) ((PAddr)(&(LDSYM_paddr_ ## x)))
#define LADDR_TO_PADDR(x) ((PAddr)((uintptr_t)(x) - (uintptr_t)LDSYM_LADDR(origin)))

// Symbols that mark parts of the kernel's address space.
extern LAddr LDSYM_laddr_kernel_pmmu_page_stack_bottom;
extern LAddr LDSYM_laddr_kernel_stack_bottom;
extern LAddr LDSYM_laddr_kernel_stack_bottom_fence;
extern LAddr LDSYM_laddr_kernel_stack_top;
extern LAddr LDSYM_laddr_kernel_stack_top_fence;
extern LAddr LDSYM_laddr_kernel_start;
extern LAddr LDSYM_laddr_origin;
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
