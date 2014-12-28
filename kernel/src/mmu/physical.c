#include "mmu/physical.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "log.h"
#include "multiboot.h"

#define LOG(...) log("PMMU", __VA_ARGS__)
#define PANIC(...) panic("PMMU", __VA_ARGS__)

// The start addresses of low and high memory.
static const uint32_t kLoMemStart = 0x00000000; // 0
static const uint32_t kHiMemStart = 0x00100000; // 1 MiB

// The stack of physical addresses.  The top of the stack is set once the size
// of the stack is known in init_mmu.
static PAddr* page_stack_top;
static PAddr* page_stack_bottom;

void init_pmmu(multiboot_info_t* mbi) {
  if (!(mbi->flags & 0x1)) {
    PANIC("multiboot doesn't provide mem_*\n");
  }

  page_stack_bottom = (PAddr*)LDSYM_LADDR(kernel_pmmu_page_stack_bottom);
  page_stack_top = page_stack_bottom;

  // Push all the physical page addresses in the lower part of memory.
  for (unsigned long i = 0; i < mbi->mem_lower / 4 - 1; i++) {
    const PAddr paddr = (PAddr)(kLoMemStart + PAGESIZE * i);
    if (paddr_within(LDSYM_PADDR(kernel_start), paddr, LDSYM_PADDR(kernel_end))) {
      continue;
    }
    *(++page_stack_top) = paddr;
  }

  // Push all the physical page addresses in the upper part of memory.
  for (unsigned long i = 0; i < mbi->mem_upper / 4 - 1; i++) {
    const PAddr paddr = (PAddr)(kHiMemStart + PAGESIZE * i);
    if (paddr_within(LDSYM_PADDR(kernel_start), paddr, LDSYM_PADDR(kernel_end))) {
      continue;
    }
    *(++page_stack_top) = paddr;
  }

  {
    const int pages = (page_stack_top - page_stack_bottom);
    LOG("%d bytes available (%d pages)\n", pages * PAGESIZE, pages);
  }
}

PAddr pmmu_get_page(void) {
  if (page_stack_top == page_stack_bottom) {
    PANIC("out of physical pages\n");
  }
  return *(page_stack_top--);
}

void pmmu_put_page(PAddr paddr) {
  *(++page_stack_top) = paddr;
}

bool pmmu_is_paddr_within_stack(PAddr paddr) {
  const PAddr bottom = LADDR_TO_PADDR(page_stack_bottom);
  const PAddr top = LADDR_TO_PADDR(page_stack_top);
  return (bottom <= paddr && paddr < top);
}
