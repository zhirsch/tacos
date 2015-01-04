#ifndef MMU_PHYSICAL_H
#define MMU_PHYSICAL_H

#include <stdbool.h>
#include <stdint.h>

#include "multiboot.h"

// Initialize the physical MMU.  Should only be called once.
void init_pmmu(multiboot_info_t* mbi);

// Acquire a physical page.
uintptr_t pmmu_get_page(void);

// Release a physical page.
void pmmu_put_page(uintptr_t paddr);

// Returns true if a physical address is within the pmmu's stack.
bool pmmu_is_paddr_within_stack(uintptr_t paddr);

#endif /* MMU_PHYSICAL_H */
