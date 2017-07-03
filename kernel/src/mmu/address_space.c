#include "mmu/address_space.h"

#include "mmu/common.h"
#include "mmu/linear.h"
#include "string.h"

static uintptr_t clone_pte(int pdi, int pti) {
	uintptr_t paddr;
  void* old = (void*)((pdi * 1024 + pti) * PAGESIZE);
  void* new = (void*)0xEFFFD000;
  mmu_map_page(new, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
  memcpy(new, old, PAGESIZE);
  paddr = lmmu_get_paddr(new);
  lmmu_unmap_page(new);
  return paddr;
}

static uintptr_t clone_pde(int pdi) {
	uintptr_t paddr;
  uint32_t* old = (uint32_t*)(0xFFC00000 + pdi * PAGESIZE);
  uint32_t* new = (uint32_t*)0xEFFFE000;
  mmu_map_page(new, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
  for (int pti = 0; pti < 1024; pti++) {
    const uintptr_t addr = (pdi << 22) | (pti << 12);
    if (addr == 0x000B8000) {
      new[pti] = old[pti];
    } else if (old[pti] & MMU_PAGE_PRESENT) {
      new[pti] = clone_pte(pdi, pti) | lmmu_get_pte_flags((void*)addr);
    } else {
      new[pti] = 0;
    }
  }
  paddr = lmmu_get_paddr(new);
  lmmu_unmap_page(new);
  return paddr;
}

uintptr_t mmu_clone_address_space(void) {
	uintptr_t paddr;
  uint32_t* old = (uint32_t*)0xFFFFF000;
  uint32_t* new = (uint32_t*)0xEFFFF000;
  mmu_map_page(new, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
  for (int pdi = 0; pdi < 1023; pdi++) {
  	if (!(old[pdi] & MMU_PAGE_PRESENT)) {
  		new[pdi] = 0;
  	} else if (old[pdi] & MMU_PAGE_USER) {
      new[pdi] = clone_pde(pdi) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;
    } else {
      new[pdi] = old[pdi];
    }
  }
  // Map the page directory to itself.
  paddr = lmmu_get_paddr(new);
  new[1023] = paddr | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;;
  lmmu_unmap_page(new);
  return paddr;
}

void mmu_free_address_space(void) {
  for (int i = 0; i < 1024; i++) {
    const uintptr_t addr_hi = (i << 22);
    const uintptr_t pde = lmmu_get_pde_flags((void*)addr_hi);
    if (!(pde & MMU_PAGE_PRESENT)) {
      // Skip non-present page directory entries.
      continue;
    }
    if (!(pde & MMU_PAGE_USER)) {
      // Don't unmap kernel page directory entries.
      continue;
    }
    for (int j = 0; j < 1024; j++) {
      const uintptr_t addr = addr_hi | (j << 12);
      const uintptr_t pte = lmmu_get_pte_flags((void*)addr);
      if (!(pte & MMU_PAGE_PRESENT)) {
        // Skip non-present page table entries.
        continue;
      }
      if (addr == 0x000B8000) {
        // Don't unmap the direct access to VGA memory.
        continue;
      }
      if (!(pte & MMU_PAGE_USER)) {
        // Don't unmap kernel page table entries.
        continue;
      }
      mmu_unmap_page((void*)addr);
    }
  }
}
