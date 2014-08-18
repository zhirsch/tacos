#include <stdint.h>

#include "dt.h"
#include "ide/ide.h"
#include "interrupts.h"
#include "kprintf.h"
#include "multiboot.h"
#include "pagedir.h"
#include "pic.h"
#include "portio.h"
#include "screen.h"
#include "snprintf.h"
#include "ssp.h"
#include "tss.h"

extern char kernel_stack[];

static struct tss kernel_tss;

static void init_paging(void);
static void init_kernel_tss(void);

void iso9660_print_paths(const uint8_t* dirrec, const char* parent) {
  char path[512] = {'\0'};
  uint8_t buffer[2048];
  uint32_t extent_pos, extent_len;
  int i;

  if (__builtin_strcmp(parent, "/") == 0) {
    snprintf(path, sizeof(path) - 1, "/%.*s", dirrec[32], dirrec+33);
  } else {
    snprintf(path, sizeof(path) - 1, "%s/%.*s", parent, dirrec[32], dirrec+33);
  }

  __builtin_memcpy(&extent_pos, dirrec + 2, 4);
  __builtin_memcpy(&extent_len, dirrec + 10, 4);
  kprintf("%02x %-40s (%08lx, %08lx)\n", dirrec[25], path, extent_pos, extent_len);
  if (!(dirrec[25] & 0x2)) {
    return;
  }

  if (!ide_read(0, 0, buffer, extent_pos, extent_len / 2)) {
    kprintf("Error!\n");
    return;
  }

  // First record is for ".", second record is for ".."; skip them.
  i = 0;
  i += buffer[i];
  i += buffer[i];
  while (buffer[i] != 0) {
    __builtin_memset(path, '\0', sizeof(path));
    if (__builtin_strcmp(parent, "/") == 0) {
      snprintf(path, sizeof(path), "/%.*s", dirrec[32], dirrec+33);
    } else {
      snprintf(path, sizeof(path), "%s/%.*s", parent, dirrec[32], dirrec+33);
    }
    iso9660_print_paths(buffer+i, path);
    i += buffer[i];
  }
}

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  // Initialize the address space.
  init_ssp();
  init_paging();
  init_kernel_tss();

  // Clear the screen.
  clearscreen();

  // Enable interrupts.
  init_interrupts();
  init_pic(0x20, 0x28);
  __asm__ __volatile__ ("sti");

  // Initialize IDE.
  init_ide();

  // Read the first non-reserved sector of the CD-ROM.
  {
    int lba = 0x10;
    while (1) {
      uint8_t buffer[2048];
      if (!ide_read(0, 0, buffer, lba, 1024)) {
        kprintf("Uh-oh @%x\n", lba);
        break;
      } else if (buffer[0] == 0xff) {
        break;
      }
      kprintf("Type           = %02x\n", buffer[0]);
      kprintf("ID             = %.*s\n", 5, buffer+1);
      kprintf("Version        = %02x\n", buffer[6]);
      if (buffer[0] == 0x01) {
        iso9660_print_paths(buffer+156, "/");
      }
      lba++;
    }
  }

  puts("Done!\n");
  __asm__ __volatile__ ("cli; hlt");
}

static struct pte pagetable[1024] __attribute__ ((aligned(0x1000)));

static void page_fault_handler(int vector, int error_code, struct tss* prev_tss) {
  uint32_t cr2;
  __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
  kprintf("Page fault! code=%08x eip=%08x addr=%08lx\n", error_code, prev_tss->eip, cr2);
  // Cause a double fault!
  *(char*)(0xB0000000) = '\0';
}

static void init_paging(void) {
  interrupt_register_handler(0xe, page_fault_handler);

  // Use PTEs for the first page.
  kernel_pagedir[0].e.table.present = 1;
  kernel_pagedir[0].e.table.rw = 1;
  kernel_pagedir[0].e.table.us = 0;
  kernel_pagedir[0].e.table.pwt = 0;
  kernel_pagedir[0].e.table.pcd = 0;
  kernel_pagedir[0].e.table.accessed = 0;
  kernel_pagedir[0].e.table.reserved1 = 0;
  kernel_pagedir[0].e.table.ps = 0;
  kernel_pagedir[0].e.table.reserved2 = 0;
  kernel_pagedir[0].e.table.addr = (((unsigned int)pagetable) - 0xC0000000) >> 12;

  // Identity map the first megabyte for easy access to the BIOS, VGA, etc.
  for (int i = 0; i < 256; i++) {
    pagetable[i].present = 1;
    pagetable[i].rw = 1;
    pagetable[i].us = 0;
    pagetable[i].pwt = 0;
    pagetable[i].pcd = 0;
    pagetable[i].accessed = 0;
    pagetable[i].dirty = 0;
    pagetable[i].pat = 0;
    pagetable[i].global = 0;
    pagetable[i].reserved1 = 0;
    pagetable[i].addr = i;
  }

  // Unmap the second page.
  __builtin_memset(kernel_pagedir + 1, 0, sizeof(kernel_pagedir[1]));

  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax");
}

static void init_kernel_tss(void) {
  gdt[5].limit_lo    = (((unsigned int)(sizeof(kernel_tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((unsigned int)(&kernel_tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((unsigned int)(sizeof(kernel_tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((unsigned int)(&kernel_tss)) & 0xFF000000) >> 24;

  kernel_tss.esp0   = (unsigned int)kernel_stack;
  kernel_tss.ss0    = 0x10;
  __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (kernel_tss.cr3));
  kernel_tss.eip    = (unsigned int)kmain;
  kernel_tss.eflags = 0x0002;
  kernel_tss.esp    = (unsigned int)kernel_stack;
  kernel_tss.cs     = 0x08;
  kernel_tss.ss     = 0x10;
  kernel_tss.ds     = 0x10;
  kernel_tss.es     = 0x10;
  kernel_tss.fs     = 0x10;
  kernel_tss.gs     = 0x10;

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
