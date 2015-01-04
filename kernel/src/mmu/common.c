#include "mmu/common.h"

#include "log.h"
#include "mmu/linear.h"
#include "mmu/physical.h"
#include "multiboot.h"
#include "string.h"

#define LOG(...) log("MMU", __VA_ARGS__)

void init_mmu(multiboot_info_t* mbi) {
  init_pmmu(mbi);
  init_lmmu();
}

void mmu_map_page(void* laddr, uint8_t flags) {
  lmmu_map_page(pmmu_get_page(), laddr, flags);
}

void mmu_unmap_page(void* laddr) {
  pmmu_put_page(lmmu_unmap_page(laddr));
}

void mmu_set_page_flags(void* laddr, uint8_t flags) {
  lmmu_set_page_flags(laddr, flags);
}

void mmu_map_system_rw_page(void* laddr) {
  mmu_map_page(laddr, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
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

static uintptr_t mmu_clone_page_table(int pt) {
  uint32_t* old = (uint32_t*)(0xFFC00000 + pt * PAGESIZE);
  uint32_t* new = (uint32_t*)0xEFFFE000;
  mmu_map_system_rw_page(new);
  for (int i = 0; i < 1024; i++) {
    if (!(old[i] & MMU_PAGE_PRESENT)) {
      new[i] = 0;
      continue;
    }
    if (!(old[i] & MMU_PAGE_USER)) {
      new[i] = old[i];
      continue;
    }
    new[i] = mmu_clone_page(pt, i) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;
  }
  return lmmu_unmap_page(new);
}

uintptr_t mmu_clone_address_space(void) {
  uint32_t* old = (uint32_t*)0xFFFFF000;
  uint32_t* new = (uint32_t*)0xEFFFF000;
  mmu_map_system_rw_page(new);
  for (int i = 0; i < 1024; i++) {
    if (!(old[i] & MMU_PAGE_PRESENT)) {
      new[i] = 0;
      continue;
    }
    if (!(old[i] & MMU_PAGE_USER)) {
      new[i] = old[i];
      continue;
    }
    new[i] = mmu_clone_page_table(i) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;
  }
  return lmmu_unmap_page(new);
}
