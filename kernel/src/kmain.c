#include <stdint.h>

#include "elf.h"
#include "dt.h"
#include "ide/ide.h"
#include "interrupts.h"
#include "iso9660.h"
#include "kmalloc.h"
#include "kprintf.h"
#include "mmu.h"
#include "multiboot.h"
#include "panic.h"
#include "pic.h"
#include "portio.h"
#include "ring3.h"
#include "screen.h"
#include "snprintf.h"
#include "ssp.h"
#include "tss.h"

static void init_kernel_tss(void);
static void start_init(const char* cmdline) __attribute__ ((noreturn));
static void exec_elf(const void* elf) __attribute__ ((noreturn));

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  char cmdline[256];

  init_ssp();

  // Clear the screen.
  clearscreen();

  // Copy the command line into a buffer with a valid virtual address.  The boot
  // loader stores a physical address in mbi->cmdline, which won't be accessible
  // after init_mmu.
  __builtin_strncpy(cmdline, (const char*)mbi->cmdline, sizeof(cmdline) - 1);
  cmdline[sizeof(cmdline)-1] = '\0';

  // Announce the OS.
  kprintf("TacOS! (%s)\n", cmdline);

  // Initialize the address space.
  init_mmu(mbi);
  init_kernel_tss();

  // Enable interrupts.
  init_interrupts();
  init_pic(0x20, 0x28);
  __asm__ __volatile__ ("sti");

  // Initialize IDE.
  init_ide();

  // Start the init program.  This doesn't return.
  start_init(cmdline);
}

static void start_init(const char* cmdline) {
  char* initpath;
  void* init_vaddr;

  // Parse the command line to find the name of the init program.
  // TODO(zhirsch): Allow for init= to be anywhere in the command line, not just
  // at the end.
  initpath = __builtin_strstr(cmdline, " init=");
  if (initpath == NULL) {
    panic("unable to find init=");
  }
  initpath += __builtin_strlen(" init=");
  kprintf("INIT: init program is \"%s\"\n", initpath);

  // Load the init program from the CD-ROM at 0-0.
  init_vaddr = iso9660_load_file_from_atapi(0, 0, initpath);
  if (init_vaddr == NULL) {
    panic("INIT: Unable to load init program\n");
  }
  kprintf("INIT: %s loaded at %08lx\n", initpath, (uintptr_t)init_vaddr);

  // Execute the init program.
  exec_elf(init_vaddr);
}

static void prepare_new_process(void* init_vaddr) {
  struct Elf32_Ehdr* ehdr;
  struct Elf32_Phdr* phdr;

  // Parse the elf header.
  ehdr = (struct Elf32_Ehdr*)init_vaddr;
  if (ehdr->e_ident[0] != EI_MAG0 ||
      ehdr->e_ident[1] != EI_MAG1 ||
      ehdr->e_ident[2] != EI_MAG2 ||
      ehdr->e_ident[3] != EI_MAG3) {
    panic("ELF: bad magic\n");
  }
  if (ehdr->e_ident[4] != ELFCLASS32) {
    panic("ELF: not ELFCLASS32\n");
  }
  if (ehdr->e_ident[5] != ELFDATA2LSB) {
    panic("ELF: not ELFDATA2LSB\n");
  }
  if (ehdr->e_ident[6] != EV_CURRENT) {
    panic("ELF: not EV_CURRENT\n");
  }
  if (ehdr->e_type != ET_EXEC) {
    panic("ELF: not ET_EXEC\n");
  }
  if (ehdr->e_machine != EM_386) {
    panic("ELF: not EM_386\n");
  }
  if (ehdr->e_version != EV_CURRENT) {
    panic("ELF: not EV_CURRENT\n");
  }
  if (ehdr->e_phoff == 0) {
    panic("ELF: no program header table\n");
  }

  kprintf("ELF: entry point: %08lx\n", ehdr->e_entry);

  // Process each of the program headers.
  phdr = (struct Elf32_Phdr*)(((uint8_t*)init_vaddr) + ehdr->e_phoff);
  for (int i = 0; i < ehdr->e_phnum; i++) {
    switch (phdr->p_type) {
    case PT_NULL:
      break;
    case PT_LOAD: {
      void* paddr = mmu_get_paddr(((const char*)ehdr)+phdr->p_offset);
      kprintf("ELF: Mapping vaddr %08lx to paddr %08lx\n", phdr->p_vaddr, (uintptr_t)paddr);
      mmu_map_page(paddr, (void*)phdr->p_vaddr, 0x3 | 0x4);
      break;
    }
    default:
      panic("ELF: unhandled program header type %lx\n", phdr->p_type);
    }
    phdr++;
  }

  // Switch to the new process in ring 3.
  switch_to_ring3((void*)ehdr->e_entry);
}

static void exec_elf(const void* elf) {
  uintptr_t* new_pagedir;
  uintptr_t* new_stack;
  struct tss* new_tss;
  unsigned int sel[2];

  // Create the new program's page directory.
  new_pagedir = kmemalign(PAGESIZE, sizeof(uintptr_t)*1024);
  // Copy the page directory entries from the current address space.
  __builtin_memcpy(new_pagedir, (const void*)0xfffff000, sizeof(*new_pagedir)*1024);
  // Map the new page directory as the last 4 MiB.
  new_pagedir[1023] = ((uintptr_t)mmu_get_paddr(new_pagedir)) | 0x7;

  // Create the new program's stack.
  // TODO(zhirsch): Don't use kmalloc to create the stack. Create guard pages.
  new_stack = kmemalign(PAGESIZE, sizeof(*new_stack) * 0x400);
  mmu_map_page(mmu_unmap_page(new_stack), new_stack, 0x7);
  new_stack[0x400] = (uintptr_t)elf;

  // Create the new program's TSS.
  new_tss = kmemalign(PAGESIZE, sizeof(struct tss));
  __builtin_memset(new_tss, 0, sizeof(*new_tss));
  new_tss->eip    = (unsigned int)prepare_new_process;
  new_tss->esp    = (unsigned int)(new_stack + 0x400 - 1);
  new_tss->eax    = (unsigned int)elf;
  new_tss->cr3    = (unsigned int)mmu_get_paddr(new_pagedir);
  new_tss->eflags = 0x0200;  // Interrupts are enabled.
  // Selectors with RPL=0 for now, switch_to_ring3 will be called later.
  new_tss->cs     = 0x08;
  new_tss->ds     = 0x10;
  new_tss->es     = 0x10;
  new_tss->fs     = 0x10;
  new_tss->gs     = 0x10;
  new_tss->ss     = 0x10;

  // Set the TSS in the GDT.
  gdt[6].limit_lo    = (((uintptr_t)(sizeof(*new_tss))) & 0x0000FFFF);
  gdt[6].base_lo     = (((uintptr_t)(new_tss)) & 0x00FFFFFF);
  gdt[6].type        = 9;
  gdt[6].reserved1   = 0;
  gdt[6].dpl         = 3;
  gdt[6].present     = 1;
  gdt[6].limit_hi    = (((uintptr_t)(sizeof(*new_tss))) & 0xFFFF0000) >> 16;
  gdt[6].available   = 0;
  gdt[6].reserved2   = 0;
  gdt[6].granularity = 0;
  gdt[6].base_hi     = (((uintptr_t)(new_tss)) & 0xFF000000) >> 24;

  // Jump to the new task with RPL=3 (although it doesn't matter since CPL=0).
  sel[0] = 0;
  sel[1] = 0x30 | 0x03;
  __asm__ __volatile__ ("ljmp *%0" : : "m" (*sel) : "memory");
  while (1) { }
}

static void init_kernel_tss(void) {
  static struct tss kernel_tss __attribute__ ((aligned(0x1000)));

  gdt[5].limit_lo    = (((uintptr_t)(sizeof(kernel_tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((uintptr_t)(&kernel_tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((uintptr_t)(sizeof(kernel_tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((uintptr_t)(&kernel_tss)) & 0xFF000000) >> 24;

  // The kernel's TSS is set to zeros because:
  //   1) There should never be a switch to this task from a different
  //      privilege level.
  // and
  //   2) The necessary fields will be filled in upon the first switch to a
  //      different task.
  // The exception to this is cr3, because the processor doesn't save it on a
  // task switch.
  __builtin_memset(&kernel_tss, 0, sizeof(kernel_tss));
  __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (kernel_tss.cr3));

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
