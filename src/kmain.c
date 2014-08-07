#include "dt.h"
#include "interrupts.h"
#include "multiboot.h"
#include "screen.h"
#include "ssp.h"
#include "tss.h"

extern char kernel_stack[];

static struct tss kernel_tss;

static void init_kernel_tss(void);

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  init_ssp();
  clearscreen();
  init_kernel_tss();
  init_interrupts();

  *(char*)(0xB0000000) = '\0';

  __asm__ __volatile__ ("cli; hlt");
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
