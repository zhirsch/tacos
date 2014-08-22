#include "mmu.h"

#include <stdint.h>

#include "interrupts.h"
#include "kprintf.h"
#include "multiboot.h"
#include "panic.h"
#include "tss.h"

// The size of each page.
#define PAGESIZE 4096

// The sentinel that marks the bottom of the paddr stack.
#define PADDR_STACK_SENTINEL 0xDEADBEEF

// The start addresses of low and high memory.
#define LO_MEM_START_ADDR 0x00000000  // 0
#define HI_MEM_START_ADDR 0x00100000  // 1 MiB

// The page directory entry or page table entry for a given address.
#define PDE(x) (*((uintptr_t*)(0xFFC00000 + 1023 * PAGESIZE + 4 * ((uintptr_t)(x)) / PAGESIZE / 1024)))
#define PTE(x) (*((uintptr_t*)(0xFFC00000                   + 4 * ((uintptr_t)(x)) / PAGESIZE)))

// Symbols that mark parts of the kernel's address space.
extern void* kernel_start;
extern void* kernel_ro_start;
extern void* kernel_rw_start;
extern void* kernel_end;
extern void* kernel_ro_end;
extern void* kernel_rw_end;

// Physical addresses that correspond to the above symbols.
static const uintptr_t kernel_start_paddr = (uintptr_t)&kernel_start - 0xC0000000;
static const uintptr_t kernel_ro_start_paddr = (uintptr_t)&kernel_ro_start - 0xC0000000;
static const uintptr_t kernel_rw_start_paddr = (uintptr_t)&kernel_rw_start - 0xC0000000;
static const uintptr_t kernel_end_paddr = (uintptr_t)&kernel_end - 0xC0000000;
static const uintptr_t kernel_ro_end_paddr = (uintptr_t)&kernel_ro_end - 0xC0000000;
static const uintptr_t kernel_rw_end_paddr = (uintptr_t)&kernel_rw_end - 0xC0000000;

// The paddr stack.
static uintptr_t* paddr_stack = (uintptr_t*)&kernel_end;

// The page fault handler.
static void page_fault_handler(int vector, int error_code, struct tss* prev_tss);

static void init_kernel_page_directory(void);
static void invlpg(void* vaddr);

void init_mmu(multiboot_info_t* mbi) {
  int pages = 0;

  if (!(mbi->flags & 0x1)) {
    panic0("MMU: multiboot doesn't provide mem_*\n");
  }

  // Populate the paddr stack.
  *(paddr_stack) = PADDR_STACK_SENTINEL;
  for (unsigned long i = 0; i < mbi->mem_lower / 4 - 1; i++) {
    const uintptr_t paddr = LO_MEM_START_ADDR + PAGESIZE * i;
    if (kernel_start_paddr <= paddr && paddr < kernel_end_paddr) {
      continue;
    }
    *(++paddr_stack) = paddr;
    pages++;
  }
  for (unsigned long i = 0; i < mbi->mem_upper / 4 - 1; i++) {
    const uintptr_t paddr = HI_MEM_START_ADDR + PAGESIZE * i;
    if (kernel_start_paddr <= paddr && paddr < kernel_end_paddr) {
      continue;
    }
    *(++paddr_stack) = paddr;
    pages++;
  }

  kprintf("MMU: %d bytes available (%d pages)\n", pages * PAGESIZE, pages);

  // Register the page fault handler.
  interrupt_register_handler(0xe, page_fault_handler);

  // Prepare the kernel's page directory.
  init_kernel_page_directory();
}

static void init_kernel_page_directory(void) {
  extern uint32_t kernel_pagedir[1024];
  uintptr_t* pte;

  // Map the last PDE to the page directory.
  kernel_pagedir[1023] = (((uintptr_t)kernel_pagedir) - 0xC0000000) | 0x3;
  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax", "memory");

  // Remove the identity mapping for the first page.
  PDE(0x0000000) = 0;

  // Identity map VGA memory.
  mmu_map_page((void*)0x000B8000, (void*)0x000B8000, 0x3);

  // Re-map the kernel with PTEs.  Map the new PTEs into a temporary location,
  // then unmap them and map them into the right location.
  pte = mmu_new_physical_page();
  mmu_map_page(pte, (void*)0xB0000000, 0x3);
  for (int i = 0; i < 1024; i++) {
    const uintptr_t paddr = kernel_start_paddr + PAGESIZE * i;
    if (kernel_ro_start_paddr <= paddr && paddr < kernel_ro_end_paddr) {
      PTE(0xB0000000 + i * PAGESIZE) = paddr | 0x1;
    } else if (kernel_rw_start_paddr <= paddr && paddr < kernel_rw_end_paddr) {
      PTE(0xB0000000 + i * PAGESIZE) = paddr | 0x3;
    } else {
      PTE(0xB0000000 + i * PAGESIZE) = 0;
    }
  }
  mmu_unmap_page((void*)0xB0000000);
  mmu_map_page(pte, (void*)0xC0000000, 0x3);
}

void* mmu_new_physical_page(void) {
  const uintptr_t paddr = *(paddr_stack--);
  if (paddr == PADDR_STACK_SENTINEL) {
    panic0("MMU: out of physical pages\n");
  }
  return (void*)paddr;
}

void mmu_map_page(void* paddr, void* vaddr, int flags) {
  if (!(PDE(vaddr) & 0x1)) {
    PDE(vaddr) = ((uintptr_t)mmu_new_physical_page()) | 0x3;
    invlpg(vaddr);
  }
  PTE(vaddr) = ((uintptr_t)paddr) | (flags & 0xFFF) | 0x1;
}

void mmu_unmap_page(void* vaddr) {
  PTE(vaddr) = 0;
  // TODO(zhirsch): Free the page containing the PTEs if they're all unmapped?
}

static void page_fault_handler(int vector, int error_code, struct tss* prev_tss) {
  uint32_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  kprintf("Page fault! code=%08x eip=%08x addr=%08lx\n", error_code, prev_tss->eip, cr2);
  // Cause a double fault!
  *(char*)(0xB0000000) = '\0';
}

static void invlpg(void* vaddr) {
  __asm__ __volatile__ ("invlpg (%0)" : : "r" (vaddr) : "memory");
}
