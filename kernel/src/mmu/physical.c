#include "mmu/physical.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ldsyms.h"
#include "log.h"
#include "mmu/mmu.h"
#include "multiboot.h"

#define LOG(...) log("PMMU", __VA_ARGS__)
#define PANIC(...) panic("PMMU", __VA_ARGS__)

// The start addresses of low and high memory.
static const uint32_t kLoMemStart = 0x00000000; // 0
static const uint32_t kHiMemStart = 0x00100000; // 1 MiB

// The stack of physical addresses.  This assumes that there is at most 4 GiB of
// memory in the system.  Each entry in the stack is one page, so 2**32 / 2**12
// = 2**20 entries are needed.
static uintptr_t g_page_stack[1<<20];
static int g_page_stack_index = 0;

void init_pmmu(multiboot_info_t* mbi) {
  if (!(mbi->flags & 0x1)) {
    PANIC("multiboot doesn't provide mem_*\n");
  }

  // Push all the physical page addresses in the lower part of memory.
  for (unsigned long i = 0; i < mbi->mem_lower / 4 - 1; i++) {
    const uintptr_t addr = kLoMemStart + PAGESIZE * i;
    if ((uintptr_t)&kernel_start <= addr && addr < (uintptr_t)&kernel_end) {
      continue;
    }
    pmmu_put_page(addr);
  }

  // Push all the physical page addresses in the upper part of memory.
  for (unsigned long i = 0; i < mbi->mem_upper / 4 - 1; i++) {
    const uintptr_t addr = kHiMemStart + PAGESIZE * i;
    if ((uintptr_t)&kernel_start <= addr && addr < (uintptr_t)&kernel_end) {
      continue;
    }
    pmmu_put_page(addr);
  }

  LOG("%d bytes available (%d pages)\n", g_page_stack_index * PAGESIZE, g_page_stack_index);
}

uintptr_t pmmu_get_page(void) {
  if (g_page_stack_index == 0) {
    PANIC("out of physical pages\n");
  }
  return g_page_stack[--g_page_stack_index];
}

void pmmu_put_page(uintptr_t addr) {
  g_page_stack[g_page_stack_index++] = addr;
}
