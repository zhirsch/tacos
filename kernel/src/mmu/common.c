#include "mmu/common.h"

#include "mmu/linear.h"
#include "mmu/physical.h"
#include "multiboot.h"

void init_mmu(multiboot_info_t* mbi) {
  init_pmmu(mbi);
  init_lmmu();
}

void mmu_map_page(void* laddr, uint8_t flags) {
  lmmu_map_page(pmmu_get_page(), laddr, flags);
}

void mmu_unmap_page(void* laddr) {
  pmmu_put_page(lmmu_unmap_page((LAddr)laddr));
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
