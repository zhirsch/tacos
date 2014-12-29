#include "mmu/linear.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "mmu/physical.h"
#include "multiboot.h"
#include "string.h"

#define LOG(...) log("LMMU", __VA_ARGS__)
#define PANIC(...) panic("LMMU", __VA_ARGS__)

// The page fault handler.
static void page_fault_handler(struct isr_frame* frame);

static uintptr_t* kPageDirectory = (uintptr_t*)0xFFFFF000;
static uintptr_t* kPageTables = (uintptr_t*)0xFFC00000;

static inline void invlpg(LAddr laddr) {
  __asm__ __volatile__ ("invlpg %0" : : "m" (*(const char*)laddr) : "memory");
}

static uintptr_t get_page_directory_index(LAddr laddr) {
  return ((uintptr_t)laddr) >> 22;
}

static inline uintptr_t get_page_directory_entry(LAddr laddr) {
  return kPageDirectory[get_page_directory_index(laddr)];
}

static inline void set_page_directory_entry(LAddr laddr, uintptr_t entry) {
  kPageDirectory[get_page_directory_index(laddr)] = entry;
  invlpg(laddr);
}

static uintptr_t get_page_table_index(LAddr laddr) {
  return ((uintptr_t)laddr) >> 12;
}

static inline uintptr_t get_page_table_entry(LAddr laddr) {
  return kPageTables[get_page_table_index(laddr)];
}

static inline void set_page_table_entry(LAddr laddr, uintptr_t entry) {
  kPageTables[get_page_table_index(laddr)] = entry;
  invlpg(laddr);
}

extern uint32_t kernel_pagedir[1024];

void init_lmmu() {
  uint32_t* temp = (uint32_t*)0xB0000000;

  interrupt_register_handler(0xe, page_fault_handler);

  // Map the last PDE to the page directory.
  kernel_pagedir[1023] = ((uintptr_t)kernel_pagedir - (uintptr_t)LDSYM_LADDR(origin)) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;
  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax", "memory");

  // Remove the identity mapping for the first page.
  set_page_directory_entry((LAddr)(0x0000000), 0);

  // Identity map VGA memory.
  lmmu_map_page((PAddr)0x000B8000, (LAddr)0x000B8000, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);

  // Re-map the kernel with PTEs.  Map the new PTEs into a temporary location,
  // then unmap them and map them into the right location.
  lmmu_map_page(pmmu_get_page(), (LAddr)temp, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
  for (int i = 0; i < 1024; i++) {
    const PAddr paddr = (PAddr)(PAGESIZE * i);
    if (paddr_within(LDSYM_PADDR(kernel_ro_start), paddr, LDSYM_PADDR(kernel_ro_end))) {
      temp[i] = (uintptr_t)paddr | MMU_PAGE_PRESENT;
    } else if (paddr_within(LDSYM_PADDR(kernel_stack_bottom_fence), paddr, LDSYM_PADDR(kernel_stack_bottom))) {
      temp[i] = 0;
    } else if (paddr_within(LDSYM_PADDR(kernel_stack_top), paddr, LDSYM_PADDR(kernel_stack_top_fence))) {
      temp[i] = 0;
    } else if (paddr_within(LDSYM_PADDR(kernel_rw_start), paddr, LDSYM_PADDR(kernel_rw_end))) {
      temp[i] = (uintptr_t)paddr | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;
    } else if (pmmu_is_paddr_within_stack(paddr)) {
      temp[i] = (uintptr_t)paddr | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;
    } else {
      temp[i] = 0;
    }
  }
  set_page_directory_entry(LDSYM_LADDR(kernel_start),
                           (uintptr_t)lmmu_unmap_page((LAddr)temp) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE);
}

void lmmu_map_page(PAddr paddr, LAddr laddr, uint8_t flags) {
  if (!(get_page_directory_entry(laddr) & MMU_PAGE_PRESENT)) {
    set_page_directory_entry(laddr, (uintptr_t)pmmu_get_page() | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER);
  }
  set_page_table_entry(laddr, (uintptr_t)paddr | (flags & 0xFFF));
}

PAddr lmmu_unmap_page(LAddr laddr) {
  const uint32_t pte = get_page_table_entry(laddr);
  if (!(pte & MMU_PAGE_PRESENT)) {
    PANIC("mmu_unmap_page(%p) is unmapped: %08lx\n", (void*)laddr, pte);
  }
  set_page_table_entry(laddr, 0);
  // TODO(zhirsch): Free the page containing the PTEs if they're all unmapped?
  return (PAddr)(pte & 0xFFFFF000);
}

void lmmu_set_page_flags(LAddr laddr, uint8_t flags) {
  const uint32_t pte = get_page_table_entry(laddr);
  if (!(pte & MMU_PAGE_PRESENT)) {
    PANIC("mmu_set_page_flags(%p) is unmapped: %08lx\n", (void*)laddr, pte);
  }
  set_page_table_entry(laddr, (pte & 0xFFFFF000) | (flags & 0xFFF));
}

void lmmu_reset_cr3(void) {
  for (unsigned int i = 0; i < 1024; i++) {
    if (i == 0) {
      for (unsigned int j = 0; j < 1024; j++) {
        const LAddr laddr = (LAddr)((i << 22) | (j << 12));
        if (((i << 22) | (j << 12)) == 0x000B8000) {
          continue;
        }
        set_page_table_entry(laddr, 0);
      }
      continue;
    }
    if ((i << 22) < 0xC0000000) {
      const LAddr laddr = (LAddr)(i << 22);
      set_page_directory_entry(laddr, 0);
    }
  }
}

PAddr lmmu_new_page_directory(void) {
  const PAddr new_pagedir_paddr = pmmu_get_page();
  uint32_t* temp = (uint32_t*)0xB0000000;
  lmmu_map_page(new_pagedir_paddr, (LAddr)temp, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);

  // Copy the last fourth of the existing page directory.  This copies the
  // kernel mappings.
  // TODO(zhirsch): Make sure that all the PDEs are populated so that every
  // process gets a consistent view.
  memcpy((void*)0xB0000C00, (void*)0xFFFFFC00, PAGESIZE / 4);

  // Map the new page directory as the last entry so that the page directory can
  // easily be refernced.
  temp[1023] = (uint32_t)new_pagedir_paddr | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;

  // Unmap the new page directory.
  lmmu_unmap_page((LAddr)temp);

  return new_pagedir_paddr;
}

PAddr lmmu_get_cr3(void) {
  uintptr_t cr3;
  __asm__ __volatile__ ("mov %%cr3, %0" : "=g" (cr3));
  return (PAddr)cr3;
}

static void page_fault_handler(struct isr_frame* frame) {
  uintptr_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  LOG("Page fault! code=%08x eip=%08lx addr=%08lx\n", frame->error_code, frame->eip, cr2);
  print_call_stack(frame->eip, frame->ebp);
  if (laddr_within(LDSYM_LADDR(kernel_stack_bottom_fence), (LAddr)cr2, LDSYM_LADDR(kernel_stack_bottom))) {
    PANIC("  kernel stack overflow\n");
  } else if (laddr_within(LDSYM_LADDR(kernel_stack_top), (LAddr)cr2, LDSYM_LADDR(kernel_stack_top_fence))) {
    PANIC("  kernel stack underflow\n");
  } else {
    PANIC("  unknown reason\n");
  }
}
