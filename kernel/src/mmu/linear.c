#include "mmu/linear.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "ldsyms.h"
#include "log.h"
#include "mmu/physical.h"
#include "multiboot.h"
#include "string.h"

#define LOG(...) log("LMMU", __VA_ARGS__)
#define PANIC(...) panic("LMMU", __VA_ARGS__)

// The page fault handler.
static void page_fault_handler(struct isr_frame* frame);

static uintptr_t* g_page_directory = (uintptr_t*)0xFFFFF000;
static uintptr_t* g_page_tables = (uintptr_t*)0xFFC00000;

static inline void invlpg(const void* addr) {
  __asm__ __volatile__ ("invlpg %0" : : "m" (*(const char*)addr) : "memory");
}

static uintptr_t get_page_directory_index(const void* addr) {
  return ((uintptr_t)addr) >> 22;
}

static inline uintptr_t get_page_directory_entry(const void* addr) {
  return g_page_directory[get_page_directory_index(addr)];
}

static inline void set_page_directory_entry(const void* addr, uintptr_t entry) {
  g_page_directory[get_page_directory_index(addr)] = entry;
  invlpg(addr);
}

static uintptr_t get_page_table_index(const void* addr) {
  return ((uintptr_t)addr) >> 12;
}

static inline uintptr_t get_page_table_entry(const void* addr) {
  return g_page_tables[get_page_table_index(addr)];
}

static inline void set_page_table_entry(const void* addr, uintptr_t entry) {
  g_page_tables[get_page_table_index(addr)] = entry;
  invlpg(addr);
}

extern uintptr_t kernel_pagedir[1024];
extern const void kernel_stack_end;

void init_lmmu() {
  interrupt_register_handler(0xe, page_fault_handler);

  // Remove the identity mapping for the first page, except for VGA memory.
  set_page_directory_entry((void*)(0x0000000), 0);
  lmmu_map_page(0x000B8000, (void*)0x000B8000, MMU_PAGE_PRESENT | MMU_PAGE_WRITE);

  // Update the page directory to reflect the right permissions and unmapped pages.
  for (int i = 0; i < 255 * 1024; i++) {
    const uintptr_t addr = 0xC0000000 + PAGESIZE * i;
    if ((uintptr_t)&kernel_ro_start <= addr && addr < (uintptr_t)&kernel_ro_end) {
      // The read only part is read only.
      g_page_tables[addr >> 12] = (addr - 0xC0000000) | MMU_PAGE_PRESENT;
    } else if (addr == (uintptr_t)&kernel_stack_end) {
      // The last page of the stack is unmapped to prevent overflow.
      g_page_tables[addr >> 12] = 0;
    } else if ((uintptr_t)&kernel_rw_start <= addr && addr < (uintptr_t)&kernel_rw_end) {
      // The writable part is writable.
      g_page_tables[addr >> 12] = (addr - 0xC0000000) | MMU_PAGE_PRESENT | MMU_PAGE_WRITE;
    } else {
      // Everything else is unmapped.
      g_page_tables[addr >> 12] = 0;
    }
  }

  // Reload cr3 to clear the tlb.
  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax", "memory");
}

void lmmu_map_page(uintptr_t paddr, void* laddr, uint8_t flags) {
  if (!(get_page_directory_entry(laddr) & MMU_PAGE_PRESENT)) {
    set_page_directory_entry(laddr, pmmu_get_page() | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER);
  }
  set_page_table_entry(laddr, paddr | (flags & 0xFFF));
}

uintptr_t lmmu_unmap_page(void* laddr) {
  const uintptr_t pte = get_page_table_entry(laddr);
  if (!(pte & MMU_PAGE_PRESENT)) {
    PANIC("mmu_unmap_page(%p) is unmapped: %08lx\n", laddr, pte);
  }
  set_page_table_entry(laddr, 0);
  // TODO(zhirsch): Free the page containing the PTEs if they're all unmapped?
  return (pte & 0xFFFFF000);
}

void lmmu_set_page_flags(void* addr, uint8_t flags) {
  const uintptr_t pte = get_page_table_entry(addr);
  if (!(pte & MMU_PAGE_PRESENT)) {
    PANIC("mmu_set_page_flags(%p) is unmapped: %08lx\n", addr, pte);
  }
  set_page_table_entry(addr, (pte & 0xFFFFF000) | (flags & 0xFFF));
}

void lmmu_reset_cr3(void) {
  for (uintptr_t i = 0; i < 1024; i++) {
    const uintptr_t addr_hi = i << 22;
    if (i == 0) {
      for (uintptr_t j = 0; j < 1024; j++) {
        const uintptr_t addr = addr_hi | (j << 12);
        if (addr == 0x000B8000) {
          continue;
        }
        set_page_table_entry((void*)addr, 0);
      }
      continue;
    }
    if (addr_hi < 0xC0000000) {
      set_page_directory_entry((void*)addr_hi, 0);
    }
  }
}

uintptr_t lmmu_get_cr3(void) {
  uintptr_t cr3;
  __asm__ __volatile__ ("mov %%cr3, %0" : "=g" (cr3));
  return cr3;
}

static void page_fault_handler(struct isr_frame* frame) {
  uintptr_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  LOG("Page fault! code=%08x eip=%08lx addr=%08lx\n", frame->error_code, frame->eip, cr2);
  print_call_stack(frame->eip, frame->ebp);
}
