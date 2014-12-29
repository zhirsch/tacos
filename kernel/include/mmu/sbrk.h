#ifndef MMU_SBRK_H
#define MMU_SBRK_H

#include <stdint.h>

void mmu_sbrk_grow(uintptr_t* cpb, uintptr_t increment);
void mmu_sbrk_shrink(uintptr_t* cpb, uintptr_t decrement);

#endif /* MMU_SBRK_H */
