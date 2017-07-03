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

static inline uintptr_t* pde(const void* addr) {
  return &g_page_directory[((uintptr_t)addr) >> 22];
}

static inline uintptr_t* pte(const void* addr) {
  return &g_page_tables[((uintptr_t)addr) >> 12];
}

extern uintptr_t kernel_pagedir[1024];
extern const void kernel_stack_end;

void init_lmmu() {
  interrupt_register_handler(0xe, page_fault_handler);

  // Remove the identity mapping for the first page, except for VGA memory.
  *pde((void*)(0x0000000)) = 0;
  invlpg(0x00000000);
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
  if (!(*pde(laddr) & MMU_PAGE_PRESENT)) {
    LOG("Mapping new page table page for %p\n", laddr);
    *pde(laddr) = pmmu_get_page() | MMU_PAGE_PRESENT | MMU_PAGE_WRITE | MMU_PAGE_USER;
    invlpg(laddr);
  }
  *pte(laddr) = paddr | (flags & 0xFFF);
  invlpg(laddr);
}

void lmmu_unmap_page(void* laddr) {
  if (!(*pte(laddr) & MMU_PAGE_PRESENT)) {
    PANIC("lmmu_unmap_page(%p) is unmapped\n", laddr);
  }
  *pte(laddr) = 0;
  invlpg(laddr);
}

uintptr_t lmmu_get_paddr(void* laddr) {
  if (!(*pte(laddr) & MMU_PAGE_PRESENT)) {
    PANIC("lmmu_get_paddr(%p) is unmapped\n", laddr);
  }
  return (*pte(laddr) & 0xFFFFF000);
}

uint8_t lmmu_get_pde_flags(void* addr) { return *pde(addr) & 0xFFF; }

uint8_t lmmu_get_pte_flags(void* addr) { return *pte(addr) & 0xFFF; }

void lmmu_set_pte_flags(void* addr, uint8_t flags) {
  if (!(*pte(addr) & MMU_PAGE_PRESENT)) {
    PANIC("mmu_set_pte_flags(%p) is unmapped\n", addr);
  }
  *pte(addr) |= flags & 0xFFF;
  invlpg(addr);
}

uintptr_t lmmu_get_cr3(void) {
  uintptr_t cr3;
  __asm__ __volatile__ ("mov %%cr3, %0" : "=g" (cr3));
  return cr3;
}

static void page_fault_handler(struct isr_frame* frame) {
  uintptr_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  LOG("Page fault! code=%08x eip=%08lx addr=%08lx\n", frame->error_code, frame->user_eip, cr2);
  print_call_stack(frame->user_eip, frame->ebp);
}
