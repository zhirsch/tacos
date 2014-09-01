#include <stdint.h>

#include "bits/fcntl.h"

#include "dt.h"
#include "elf.h"
#include "fpu.h"
#include "ide/ide.h"
#include "interrupts.h"
#include "iso9660.h"
#include "kmalloc.h"
#include "log.h"
#include "mmu.h"
#include "multiboot.h"
#include "pic.h"
#include "portio.h"
#include "process.h"
#include "screen.h"
#include "snprintf.h"
#include "ssp.h"
#include "string.h"
#include "syscall.h"
#include "tss.h"
#include "tty.h"

#define LOG(...) log("KMAIN", __VA_ARGS__)
#define PANIC(...) panic("KMAIN", __VA_ARGS__)

static void init_tss(void);
static void start_init(const char* cmdline) __attribute__ ((noreturn));
static void exec_elf(const void* elf) __attribute__ ((noreturn));

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  char cmdline[256];

  init_ssp();

  // Initialize the consoles.
  init_tty();
  screen_clear();

  // Copy the command line into a buffer with a valid virtual address.  The boot
  // loader stores a physical address in mbi->cmdline, which won't be accessible
  // after init_mmu.
  strncpy(cmdline, (const char*)mbi->cmdline, sizeof(cmdline) - 1);
  cmdline[sizeof(cmdline)-1] = '\0';

  // Announce the OS.
  screen_writef("TacOS! (%s)\n", cmdline);

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
  initpath = strstr(cmdline, " init=");
  if (initpath == NULL) {
    PANIC("unable to find init=");
  }
  initpath += strlen(" init=");
  LOG("init program is \"%s\"\n", initpath);

  // Load the init program from the CD-ROM at 0-0.
  init_vaddr = iso9660_load_file_from_atapi(0, 0, initpath);
  if (init_vaddr == NULL) {
    PANIC("Unable to load init program\n");
  }
  LOG("%s loaded at %08lx\n", initpath, (uintptr_t)init_vaddr);

  // Execute the init program.
  exec_elf(init_vaddr);
}


static void exec_elf(const void* elf) {
  struct process* new_process;
  struct Elf32_Ehdr* ehdr;
  struct Elf32_Phdr* phdr;

  // Disable interrupts.
  __asm__ __volatile__ ("cli");

  // Create space for the new process struct.
  new_process = kmalloc(sizeof(*new_process));
  memset(new_process, 0, sizeof(*new_process));
  new_process->pid = 1;
  new_process->ppid = 0;
  new_process->pgid = 1;
  new_process->uid = 0;
  new_process->euid = 0;
  new_process->gid = 0;
  new_process->egid = 0;
  new_process->tty = 0;
  new_process->cwd = "/";
  new_process->tss.cs = 0x1b;
  new_process->tss.ss = 0x23;
  new_process->tss.ds = 0x23;
  new_process->tss.es = 0x23;
  new_process->tss.fs = 0x23;
  new_process->tss.gs = 0x23;
  new_process->tss.eflags = 0x0200;

  // Set the new process as the foreground process group for the tty.
  ttys[0].pgid = 1;

  // stdin
  new_process->fds[0].used = 1;
  new_process->fds[0].flags = O_RDONLY;
  new_process->fds[0].tty = 0;
  // stdout
  new_process->fds[1].used = 1;
  new_process->fds[1].flags = O_WRONLY;
  new_process->fds[1].tty = 0;
  // stderr
  new_process->fds[2].used = 1;
  new_process->fds[2].flags = O_WRONLY;
  new_process->fds[2].tty = 0;

  // Create a new address space for the process.
  new_process->tss.cr3 = mmu_new_page_directory();
  mmu_switch_page_directory(new_process->tss.cr3);

  // Parse the elf header.
  ehdr = (struct Elf32_Ehdr*)elf;
  if (ehdr->e_ident[0] != EI_MAG0 ||
      ehdr->e_ident[1] != EI_MAG1 ||
      ehdr->e_ident[2] != EI_MAG2 ||
      ehdr->e_ident[3] != EI_MAG3) {
    PANIC("Bad magic\n");
  }
  if (ehdr->e_ident[4] != ELFCLASS32) {
    PANIC("Not ELFCLASS32\n");
  }
  if (ehdr->e_ident[5] != ELFDATA2LSB) {
    PANIC("Not ELFDATA2LSB\n");
  }
  if (ehdr->e_ident[6] != EV_CURRENT) {
    PANIC("Not EV_CURRENT\n");
  }
  if (ehdr->e_type != ET_EXEC) {
    PANIC("Not ET_EXEC\n");
  }
  if (ehdr->e_machine != EM_386) {
    PANIC("Not EM_386\n");
  }
  if (ehdr->e_version != EV_CURRENT) {
    PANIC("Not EV_CURRENT\n");
  }
  if (ehdr->e_phoff == 0) {
    PANIC("No program header table\n");
  }

  LOG("Entry point: %08lx\n", ehdr->e_entry);
  new_process->tss.eip = ehdr->e_entry;

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
        PANIC("Unknown flags: %08lx\n", phdr->p_flags);
      }
      while (dst < (phdr->p_vaddr + phdr->p_filesz)) {
        const uintptr_t paddr = (uintptr_t)mmu_get_paddr((void*)src);
        LOG("Mapping vaddr %08lx to paddr %08lx for %s\n", dst, paddr,
            (perm == 0x1) ? "TEXT" : "DATA");
        // TODO(zhirsch): Duplicate pages that overlap segments.
        mmu_map_page((void*)paddr, (void*)dst, perm | 0x4);
        src += PAGESIZE;
        dst += PAGESIZE;
      }
      // Allocate additional pages for the BSS and zero it out.
      while (dst < (phdr->p_vaddr + phdr->p_memsz)) {
        const uintptr_t paddr = (uintptr_t)mmu_acquire_physical_page();
        LOG("Mapping vaddr %08lx to paddr %08lx for BSS\n", dst, paddr);
        mmu_map_page((void*)paddr, (void*)dst, 0x3 | 0x4);
        dst += PAGESIZE;
      }
      if (bss_size > 0) {
        LOG("Clearing %08lx bytes of the BSS starting at %08lx\n", bss_size, bss_base);
        memset((void*)bss_base, 0, bss_size);
      }
      break;
    }
    default:
      PANIC("unhandled program header type %lx\n", phdr->p_type);
    }
    phdr++;
  }

  // Set the break to the end of the last segment, aligned to the next page.
  new_process->program_break  = (phdr - 1)->p_vaddr + (phdr - 1)->p_memsz;
  new_process->program_break &= ~(PAGESIZE - 1);
  new_process->program_break += PAGESIZE;

  // Create the new program's stack.
  new_process->tss.esp = (uintptr_t)mmu_new_stack((void*)0xBF000000, 16, 1);
  LOG("Stack is at %08lx\n", (uintptr_t)new_process->tss.esp);

  // Prepare argc, argv, and envp.
  {
    char* vaddr = (char*)0xBE000000;
    new_process->args_paddr = (uintptr_t)mmu_acquire_physical_page();
    mmu_map_page((void*)new_process->args_paddr, vaddr, 0x1 | 0x2 | 0x4);

    // argv[0]
    vaddr[0x00] = 'd';
    vaddr[0x01] = 'a';
    vaddr[0x02] = 's';
    vaddr[0x03] = 'h';
    vaddr[0x04] = '\0';
    // argv[1]
    vaddr[0x05] = '-';
    vaddr[0x06] = 'i';
    vaddr[0x07] = '\0';

    // envp is empty.
    vaddr[0x80] = 'O';
    vaddr[0x81] = 'S';
    vaddr[0x82] = '=';
    vaddr[0x83] = 'T';
    vaddr[0x84] = 'a';
    vaddr[0x85] = 'c';
    vaddr[0x86] = 'O';
    vaddr[0x87] = 'S';
    vaddr[0x88] = '\0';

    // argv
    *((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 0) = (uintptr_t)(vaddr + 0x00);
    *((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 1) = (uintptr_t)(vaddr + 0x05);
    *((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 2) = (uintptr_t)NULL;

    // envp
    *((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 3) = (uintptr_t)(vaddr + 0x80);
    *((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 4) = (uintptr_t)NULL;

    // Set the stack.
    ((uintptr_t*)new_process->tss.esp)[-1] = (uintptr_t)((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 3);  // envp
    ((uintptr_t*)new_process->tss.esp)[-2] = (uintptr_t)((uintptr_t*)vaddr + 0x800 / sizeof(uintptr_t) + 0);  // argv
    ((uintptr_t*)new_process->tss.esp)[-3] = 1;  // argc
    new_process->tss.esp -= 3 * sizeof(uintptr_t);
  }

  // Set the new process as the active process.
  current_process = new_process;

  // Switch to the new process in ring 3.
  {
    extern void switch_to_ring3(uint16_t cs, uint32_t eip, uint16_t ss, uint32_t esp, uint32_t eflags) __attribute__ ((noreturn));
    switch_to_ring3(new_process->tss.cs,
                    new_process->tss.eip,
                    new_process->tss.ss,
                    new_process->tss.esp,
                    new_process->tss.eflags);
  }
}

#if 0
static void switch_to_ring3(struct process* process) {
  // Set the data selectors.
  __asm__ __volatile__ ("mov %0, %%ds\n"
                        "mov %1, %%es\n"
                        "mov %2, %%fs\n"
                        "mov %3, %%gs"
                        :
                        : "g" (process->tss.ds),
                          "g" (process->tss.es),
                          "g" (process->tss.fs),
                          "g" (process->tss.gs));

  // Push ss and esp.
  __asm__ __volatile__ ("push %0\n"
                        "push %1"
                        :
                        : "g" (process->tss.ss),
                          "g" (process->tss.esp));

  // Push eflags.
  __asm__ __volatile__ ("pushf\n"
                        "pop %%eax\n"
                        "or %0, %%eax\n"
                        "push %%eax"
                        :
                        : "g" (process->tss.eflags)
                        : "eax");

  // Push cs and eip.
  __asm__ __volatile__ ("push %0\n"
                        "push %1"
                        :
                        : "g" (process->tss.cs),
                          "g" (process->tss.eip));

  // Switch to ring3
  __asm__ __volatile__ ("iret"
                        :
                        :
                        : "memory");

  while (1) { }
}
#endif

static void init_tss(void) {
  static struct tss tss __attribute__ ((aligned(PAGESIZE)));
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

  memset(&tss, 0, sizeof(tss));
  tss.ss0  = 0x10;
  tss.esp0 = (uintptr_t)&kernel_stack_top;

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
