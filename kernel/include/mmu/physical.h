#ifndef MMU_PHYSICAL_H
#define MMU_PHYSICAL_H

#include <stdbool.h>
#include <stdint.h>

#include "mmu/common.h"
#include "multiboot.h"

// Initialize the physical MMU.  Should only be called once.
void init_pmmu(multiboot_info_t* mbi);

// Acquire a physical page.
PAddr pmmu_get_page(void);

// Release a physical page.
void pmmu_put_page(PAddr paddr);

// Returns true if a physical address is within the pmmu's stack.
bool pmmu_is_paddr_within_stack(PAddr paddr);

#endif /* MMU_PHYSICAL_H */
