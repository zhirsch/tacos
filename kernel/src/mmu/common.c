#include "mmu/common.h"

#include "log.h"
#include "mmu/linear.h"
#include "mmu/physical.h"
#include "multiboot.h"
#include "string.h"

#define LOG(...) log("MMU", __VA_ARGS__)
#define PANIC(...) panic("MMU", __VA_ARGS__)

void init_mmu(multiboot_info_t* mbi) {
  init_pmmu(mbi);
  init_lmmu();
}

uintptr_t mmu_map_page(void* laddr, uint8_t flags) {
  uintptr_t paddr = pmmu_get_page();
  lmmu_map_page(paddr, laddr, flags);
  return paddr;
}

void mmu_unmap_page(void* laddr) {
  pmmu_put_page(lmmu_unmap_page(laddr));
}

void mmu_set_page_flags(void* laddr, uint8_t flags) {
  lmmu_set_page_flags(laddr, flags);
}

uintptr_t mmu_map_system_rw_page(void* laddr) {
  return mmu_map_page(laddr, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
}

void mmu_map_user_rw_page(void* laddr) {
  mmu_map_page(laddr, MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER);
}

static uintptr_t mmu_clone_page(int pt, int pg) {
  void* old = (void*)((pt * 1024 + pg) * PAGESIZE);
  void* new = (void*)0xEFFFD000;
  LOG("Cloning page %p\n", old);
  mmu_map_system_rw_page(new);
  memcpy(new, old, PAGESIZE);
  return lmmu_unmap_page(new);
}

static uintptr_t mmu_clone_page_table(int pt, int attrs) {
  uint32_t* old = (uint32_t*)(0xFFC00000 + pt * PAGESIZE);
  uint32_t* new = (uint32_t*)0xEFFFE000;
  mmu_map_system_rw_page(new);
  for (int i = 0; i < 1024; i++) {
    const uintptr_t addr = (pt << 22) | (i << 12);
    if (addr == 0x000B8000) {
      new[i] = old[i];
    } else if (old[i] & MMU_PAGE_PRESENT) {
      new[i] = mmu_clone_page(pt, i) | attrs;
    } else {
      new[i] = 0;
    }
  }
  return lmmu_unmap_page(new);
}

uintptr_t mmu_clone_address_space(void) {
  uint32_t* old = (uint32_t*)0xFFFFF000;
  uint32_t* new = (uint32_t*)0xEFFFF000;
  uintptr_t new_pde_paddr = mmu_map_system_rw_page(new);
  // Copy the non-kernel pages.
  for (int i = 0; i < 768; i++) {
    if (old[i] & MMU_PAGE_PRESENT) {
      // TODO: copy the attrs.
      const int attrs = MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;
      new[i] = mmu_clone_page_table(i, attrs) | attrs;
    } else {
      new[i] = 0;
    }
  }
  // Map the exact same kernel pages.
  for (int i = 768; i < 1023; i++) {
    if (old[i] & MMU_PAGE_PRESENT) {
      new[i] = old[i];
    }
  }
  // Map the page directory to itself.
  new[1023] = new_pde_paddr | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;
  return lmmu_unmap_page(new);
}

void mmu_free_address_space(uintptr_t cr3) {
  LOG("mmu_free_address_space not implemented\n");
}
