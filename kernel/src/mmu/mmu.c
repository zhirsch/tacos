#include "mmu/mmu.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "ldsyms.h"
#include "log.h"
#include "mmu/physical.h"
#include "multiboot.h"
#include "string.h"

#define LOG(...) log("MMU", __VA_ARGS__)
#define PANIC(...) panic("MMU", __VA_ARGS__)

// The page fault handler.
static void page_fault_handler(struct isr_frame* frame);

static uintptr_t* g_page_directory = (uintptr_t*)0xFFFFF000;
static uintptr_t* g_page_tables = (uintptr_t*)0xFFC00000;

static void invlpg(const void* addr) {
  __asm__ __volatile__ ("invlpg %0" : : "m" (*(const char*)addr) : "memory");
}

static uintptr_t* pde(const void* addr) {
  return &g_page_directory[((uintptr_t)addr) >> 22];
}

static uintptr_t* pte(const void* addr) {
  return &g_page_tables[((uintptr_t)addr) >> 12];
}

static bool is_pde_present(const void* addr) {
  return *pde(addr) & kPagePresent;
}

static bool is_pte_present(const void* addr) {
  return *pte(addr) & kPagePresent;
}

extern uintptr_t kernel_pagedir[1024];
extern const void kernel_stack_end;

void init_mmu(multiboot_info_t* mbi) {
  init_pmmu(mbi);

  interrupt_register_handler(0xe, page_fault_handler);

  // Remove the identity mapping for the first page.
  *pde((void*)0x00000000) = 0;

  // Identity map VGA memory.
  assert(!is_pde_present((void*)0x000B8000));
  *pde((void*)0x000B8000) = pmmu_get_page() | kPagePresent | kPageWrite | kPageUser;
  invlpg((void*)0x000B8000);
  assert(!is_pte_present((void*)0x000B8000));
  *pte((void*)0x000B8000) = 0x000B8000 | kPagePresent | kPageWrite;
  invlpg((void*)0x000B8000);

  // Update the page directory to reflect the right permissions and unmapped pages.
  for (int i = 0; i < 255 * 1024; i++) {
    const uintptr_t addr = 0xC0000000 + PAGESIZE * i;
    if ((uintptr_t)&kernel_ro_start <= addr && addr < (uintptr_t)&kernel_ro_end) {
      // The read only part is read only.
      *pte((void*)addr) = (addr - 0xC0000000) | kPagePresent;
    } else if (addr == (uintptr_t)&kernel_stack_end) {
      // The last page of the stack is unmapped to prevent overflow.
      *pte((void*)addr) = 0;
    } else if ((uintptr_t)&kernel_rw_start <= addr && addr < (uintptr_t)&kernel_rw_end) {
      // The writable part is writable.
      *pte((void*)addr) = (addr - 0xC0000000) | kPagePresent | kPageWrite;
    } else {
      // Everything else is unmapped.
      *pte((void*)addr) = 0;
    }
    //invlpg((void*)addr);
  }

  // Reload cr3 to clear the tlb.
  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax", "memory");
}

void mmu_map_page(void* addr, enum PageFlags flags) {
  if (!is_pde_present(addr)) {
    *pde(addr) = pmmu_get_page() | kPagePresent | kPageWrite | kPageUser;
    invlpg(addr);
  }
  assert(!is_pte_present(addr));
  *pte(addr) = pmmu_get_page() | flags;
  invlpg(addr);
}

void mmu_unmap_page(void* addr) {
  if (!is_pte_present(addr)) {
    PANIC("mmu_unmap_page(%p) is unmapped\n", addr);
  }
  *pte(addr) = 0;
  invlpg(addr);
}

uintptr_t mmu_get_paddr(void* addr) {
  if (!is_pte_present(addr)) {
    PANIC("mmu_get_paddr(%p) is unmapped\n", addr);
  }
  return (*pte(addr) & 0xFFFFF000);
}

enum PageFlags mmu_get_pde_flags(void* addr) { return *pde(addr) & 0xFFF; }

enum PageFlags mmu_get_pte_flags(void* addr) { return *pte(addr) & 0xFFF; }

void mmu_set_pte_flags(void* addr, enum PageFlags flags) {
  if (!is_pte_present(addr)) {
    PANIC("mmu_set_pte_flags(%p) is unmapped\n", addr);
  }
  *pte(addr) |= flags;
  invlpg(addr);
}

uintptr_t mmu_get_cr3(void) {
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
