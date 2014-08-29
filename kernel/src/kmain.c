#include <stdint.h>

#include "dt.h"
#include "elf.h"
#include "fpu.h"
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
#include "syscall/init.h"
#include "tss.h"

static void init_tss(void);
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
  init_tss();

  // Initialize the FPU.
  init_fpu();

  // Enable interrupts.
  init_interrupts();
  init_pic(0x20, 0x28);
  init_syscalls();
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

static void exec_elf(const void* elf) {
  uintptr_t* new_stack;
  struct Elf32_Ehdr* ehdr;
  struct Elf32_Phdr* phdr;

  // Parse the elf header.
  ehdr = (struct Elf32_Ehdr*)elf;
  if (ehdr->e_ident[0] != EI_MAG0 ||
      ehdr->e_ident[1] != EI_MAG1 ||
      ehdr->e_ident[2] != EI_MAG2 ||
      ehdr->e_ident[3] != EI_MAG3) {
    panic("ELF: Bad magic\n");
  }
  if (ehdr->e_ident[4] != ELFCLASS32) {
    panic("ELF: Not ELFCLASS32\n");
  }
  if (ehdr->e_ident[5] != ELFDATA2LSB) {
    panic("ELF: Not ELFDATA2LSB\n");
  }
  if (ehdr->e_ident[6] != EV_CURRENT) {
    panic("ELF: Not EV_CURRENT\n");
  }
  if (ehdr->e_type != ET_EXEC) {
    panic("ELF: Not ET_EXEC\n");
  }
  if (ehdr->e_machine != EM_386) {
    panic("ELF: Not EM_386\n");
  }
  if (ehdr->e_version != EV_CURRENT) {
    panic("ELF: Not EV_CURRENT\n");
  }
  if (ehdr->e_phoff == 0) {
    panic("ELF: No program header table\n");
  }

  kprintf("ELF: Entry point: %08lx\n", ehdr->e_entry);

  // Process each of the program headers.
  phdr = (struct Elf32_Phdr*)(((uint8_t*)elf) + ehdr->e_phoff);
  for (int i = 0; i < ehdr->e_phnum; i++) {
    switch (phdr->p_type) {
    case PT_NULL:
      break;
    case PT_LOAD: {
      const uint32_t bss_base = phdr->p_vaddr + phdr->p_filesz;
      const uint32_t bss_size = phdr->p_memsz - phdr->p_filesz;
      const uintptr_t dst_base = phdr->p_vaddr & 0xfffff000;
      uintptr_t src = ((uintptr_t)ehdr + phdr->p_offset) & 0xfffff000;
      uintptr_t dst = dst_base;
      uint8_t perm;
      switch (phdr->p_flags) {
      case 0x5:
        perm = 0x1;  // r-x
        break;
      case 0x6:
        perm = 0x3;  // rw-
        break;
      default:
        panic("ELF: Unknown flags: %08lx\n", phdr->p_flags);
      }
      while (dst < (phdr->p_vaddr & 0xfffff000) + phdr->p_filesz) {
        const uintptr_t paddr = (uintptr_t)mmu_get_paddr((void*)src);
        kprintf("ELF: Mapping vaddr %08lx to paddr %08lx for %s\n", dst, paddr,
                (perm == 0x1) ? "TEXT" : "DATA");
        // TODO(zhirsch): Duplicate pages that overlap segments.
        mmu_map_page((void*)paddr, (void*)dst, perm | 0x4);
        src += PAGESIZE;
        dst += PAGESIZE;
      }
      // Allocate additional pages for the BSS and zero it out.
      while (dst < (phdr->p_vaddr & 0xfffff000) + phdr->p_memsz) {
        const uintptr_t paddr = (uintptr_t)mmu_acquire_physical_page();
        kprintf("ELF: Mapping vaddr %08lx to paddr %08lx for BSS\n", dst, paddr);
        mmu_map_page((void*)paddr, (void*)dst, 0x3 | 0x4);
        dst += PAGESIZE;
      }
      if (bss_size > 0) {
        kprintf("ELF: Clearing %08lx bytes of the BSS starting at %08lx\n", bss_size, bss_base);
        __builtin_memset((void*)bss_base, 0, bss_size);
      }
      break;
    }
    default:
      panic("ELF: unhandled program header type %lx\n", phdr->p_type);
    }
    phdr++;
  }

  // Create the new program's stack.
  new_stack = mmu_new_stack((void*)0xBF400000, 16, 1);
  kprintf("ELF: Stack is at %08lx\n", (uintptr_t)new_stack);

  // Switch to the new process in ring 3.
  switch_to_ring3((const void*)ehdr->e_entry, new_stack);
}

static void init_tss(void) {
  static struct tss tss __attribute__ ((aligned(0x1000)));
  extern void* kernel_stack_top;

  gdt[5].limit_lo    = (((uintptr_t)(sizeof(tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((uintptr_t)(&tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((uintptr_t)(sizeof(tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((uintptr_t)(&tss)) & 0xFF000000) >> 24;

  __builtin_memset(&tss, 0, sizeof(tss));
  tss.ss0  = 0x10;
  tss.esp0 = (uintptr_t)&kernel_stack_top;
  __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (tss.cr3));

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
