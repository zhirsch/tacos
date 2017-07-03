#ifndef MMU_ADDRESS_SPACE_H
#define MMU_ADDRESS_SPACE_H

#include <stdint.h>

// Makes a copy of the current address space.
uintptr_t mmu_clone_address_space(void);

// Frees all the pages in the current address space.
void mmu_free_address_space(void);

#endif  /* MMU_ADDRESS_SPACE_H */
