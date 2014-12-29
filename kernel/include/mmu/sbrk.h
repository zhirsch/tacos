#ifndef MMU_SBRK_H
#define MMU_SBRK_H

#include <stdint.h>

void mmu_sbrk_grow(uintptr_t* cpb, uintptr_t increment);
void mmu_sbrk_shrink(uintptr_t* cpb, uintptr_t decrement);

// The "morecore" function for the kernel, to grow the kernel's program break.
// Used by kmalloc.
// TODO: Implement my own kmalloc and this (probably) won't be needed.
void* mmu_kmorecore(intptr_t increment);

#endif /* MMU_SBRK_H */
