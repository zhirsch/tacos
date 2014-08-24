#include "mmu.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "multiboot.h"
#include "panic.h"
#include "tss.h"

// The sentinel that marks the bottom of the paddr stack.
#define PADDR_STACK_SENTINEL 0xDEADBEEF

// The start addresses of low and high memory.
#define LO_MEM_START_ADDR 0x00000000  // 0
#define HI_MEM_START_ADDR 0x00100000  // 1 MiB

// Convert a virtual address to a physical address.
#define PADDR(x) (((uintptr_t)(x)) - 0xC0000000)

// The page directory entry or page table entry for a given address.
#define PDE(x) (*((uintptr_t*)(0xFFC00000 + 1023 * PAGESIZE + 4 * (((uintptr_t)(x)) / PAGESIZE / 1024))))
#define PTE(x) (*((uintptr_t*)(0xFFC00000                   + 4 * (((uintptr_t)(x)) / PAGESIZE))))

// Symbols that mark parts of the kernel's address space.
extern void* kernel_start;
extern void* kernel_ro_start;
extern void* kernel_rw_start;
extern void* kernel_end;
extern void* kernel_ro_end;
extern void* kernel_rw_end;
extern void* paddr_stack_bottom;

// Physical addresses that correspond to the above symbols.
static const uintptr_t kernel_start_paddr       = PADDR(&kernel_start);
static const uintptr_t kernel_ro_start_paddr    = PADDR(&kernel_ro_start);
static const uintptr_t kernel_rw_start_paddr    = PADDR(&kernel_rw_start);
static const uintptr_t kernel_end_paddr         = PADDR(&kernel_end);
static const uintptr_t kernel_ro_end_paddr      = PADDR(&kernel_ro_end);
static const uintptr_t kernel_rw_end_paddr      = PADDR(&kernel_rw_end);
static const uintptr_t paddr_stack_bottom_paddr = PADDR(&paddr_stack_bottom);

// The stack of physical addresses.  The top of the stack is set once the size
// of the stack is known in init_mmu.
static uintptr_t  paddr_stack_top_paddr;
static uintptr_t* paddr_stack = (uintptr_t*)&paddr_stack_bottom;

// The page fault handler.
static void page_fault_handler(int vector, int error_code, struct tss* prev_tss);

static inline void invlpg(void* vaddr) {
  __asm__ __volatile__ ("invlpg %0" : : "m" (*(const char*)vaddr) : "memory");
}

static void init_paddr_stack(multiboot_info_t* mbi);
static void init_paging(void);

void init_mmu(multiboot_info_t* mbi) {
  if (!(mbi->flags & 0x1)) {
    panic("MMU: multiboot doesn't provide mem_*\n");
  }

  // Register the page fault handler.
  interrupt_register_handler(0xe, page_fault_handler);

  init_paddr_stack(mbi);
  init_paging();
}

static void init_paddr_stack(multiboot_info_t* mbi) {
  int pages = 0;

  // The last entry in the stack is the sentinel so that underflow can be
  // detected.
  *(paddr_stack) = PADDR_STACK_SENTINEL;

  // Push all the physical page addresses in the lower part of memory.
  for (unsigned long i = 0; i < mbi->mem_lower / 4 - 1; i++) {
    const uintptr_t paddr = LO_MEM_START_ADDR + PAGESIZE * i;
    if (kernel_start_paddr <= paddr && paddr < kernel_end_paddr) {
      continue;
    }
    *(++paddr_stack) = paddr;
    pages++;
  }

  // Push all the physical page addresses in the upper part of memory.
  for (unsigned long i = 0; i < mbi->mem_upper / 4 - 1; i++) {
    const uintptr_t paddr = HI_MEM_START_ADDR + PAGESIZE * i;
    if (kernel_start_paddr <= paddr && paddr < kernel_end_paddr) {
      continue;
    }
    *(++paddr_stack) = paddr;
    pages++;
  }

  // Set the top of the stack.
  paddr_stack_top_paddr = PADDR(paddr_stack);

  kprintf("MMU: %d bytes available (%d pages)\n", pages * PAGESIZE, pages);
}

static void init_paging(void) {
  extern uint32_t kernel_pagedir[1024];

  // Map the last PDE to the page directory.
  kernel_pagedir[1023] = PADDR(kernel_pagedir) | 0x3;
  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax", "memory");

  // Remove the identity mapping for the first page.
  PDE(0x0000000) = 0;
  invlpg(0x00000000);

  // Identity map VGA memory.
  mmu_map_page((void*)0x000B8000, (void*)0x000B8000, 0x3);

  // Re-map the kernel with PTEs.  Map the new PTEs into a temporary location,
  // then unmap them and map them into the right location.
  mmu_map_page(mmu_acquire_physical_page(), (void*)0xB0000000, 0x3);
  for (int i = 0; i < 1024; i++) {
    const uintptr_t paddr = PAGESIZE * i;
    if (kernel_ro_start_paddr <= paddr && paddr < kernel_ro_end_paddr) {
      *(((uintptr_t*)0xB0000000) + i) = paddr | 0x1;
    } else if (kernel_rw_start_paddr <= paddr && paddr < kernel_rw_end_paddr) {
      *(((uintptr_t*)0xB0000000) + i) = paddr | 0x3;
    } else if (paddr_stack_bottom_paddr <= paddr && paddr < paddr_stack_top_paddr) {
      *(((uintptr_t*)0xB0000000) + i) = paddr | 0x3;
    } else {
      *(((uintptr_t*)0xB0000000) + i) = 0;
    }
  }
  PDE(&kernel_start) = ((uintptr_t)mmu_unmap_page((void*)0xB0000000)) | 0x3;
  invlpg(&kernel_start);
}

void* mmu_acquire_physical_page(void) {
  const uintptr_t paddr = *(paddr_stack--);
  if (paddr == PADDR_STACK_SENTINEL) {
    panic("MMU: out of physical pages\n");
  }
  return (void*)paddr;
}

void mmu_release_physical_page(void* paddr) {
  *(++paddr_stack) = (uintptr_t)paddr;
}

void mmu_map_page(void* paddr, void* vaddr, int flags) {
  if (!(PDE(vaddr) & 0x1)) {
    PDE(vaddr) = ((uintptr_t)mmu_acquire_physical_page()) | 0x3;
    invlpg(vaddr);
  }
  PTE(vaddr) = ((uintptr_t)paddr) | (flags & 0xFFF) | 0x1;
  invlpg(vaddr);
}

void* mmu_unmap_page(void* vaddr) {
  const uint32_t pte = PTE(vaddr);
  void* paddr;
  if (!(pte & 0x1)) {
    panic("MMU: mmu_unmap_page(%08lx) is unmapped: %08lx\n", (uintptr_t)vaddr, pte);
  }
  paddr = (void*)(pte & 0xfffff000);
  PTE(vaddr) = 0;
  invlpg(vaddr);
  // TODO(zhirsch): Free the page containing the PTEs if they're all unmapped?
  return paddr;
}

static uintptr_t heap_end_vaddr = 0xE0000000;

void* ksbrk(intptr_t increment) {
  void* vaddr = (void*)heap_end_vaddr;
  // Make sure that a multiple of a page was requested.
  if (increment & (PAGESIZE - 1)) {
    panic("MMU: ksbrk(%lx) is not page aligned\n", increment);
  } else if (increment < 0) {
    // Shrink the heap.
    for (intptr_t i = 0; i < -increment; i += PAGESIZE) {
      void* paddr;
      heap_end_vaddr -= PAGESIZE;
      paddr = mmu_unmap_page((void*)heap_end_vaddr);
      mmu_release_physical_page(paddr);
    }
  } else if (increment > 0) {
    // Grow the heap.
    for (intptr_t i = 0; i < increment; i += PAGESIZE) {
      void* paddr;
      paddr = mmu_acquire_physical_page();
      mmu_map_page(paddr, (void*)heap_end_vaddr, 0x3);
      heap_end_vaddr += PAGESIZE;
    }
  }
  return vaddr;
}

static void page_fault_handler(int vector, int error_code, struct tss* prev_tss) {
  uint32_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  kprintf("Page fault! code=%08x eip=%08x addr=%08lx\n", error_code, prev_tss->eip, cr2);
  // Cause a double fault!
  *(char*)(0xB0000000) = '\0';
}
