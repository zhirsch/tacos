#include "interrupts.h"

#include "dt.h"
#include "kprintf.h"
#include "tss.h"

extern char kernel_stack[];

static struct tss intr_tss[256];

extern void (*(isr_array[]))(void);

void init_interrupts(void) {
  for (int i = 0; i < 256; i++) {
    // Add the TSS descriptor to the GDT.
    const int j = i + 7;
    gdt[j].limit_lo    = (((unsigned int)(sizeof(*intr_tss))) & 0x0000FFFF);
    gdt[j].base_lo     = (((unsigned int)(intr_tss + i)) & 0x00FFFFFF);
    gdt[j].type        = 9;
    gdt[j].reserved1   = 0;
    gdt[j].dpl         = 0;
    gdt[j].present     = 1;
    gdt[j].limit_hi    = (((unsigned int)(sizeof(*intr_tss))) & 0xFFFF0000) >> 16;
    gdt[j].available   = 0;
    gdt[j].reserved2   = 0;
    gdt[j].granularity = 0;
    gdt[j].base_hi     = (((unsigned int)(intr_tss + i)) & 0xFF000000) >> 24;

    // Add the task gate to the IDT.
    idt[i].reserved1   = 0;
    idt[i].selector    = (i + 7) << 3;
    idt[i].reserved2   = 0;
    idt[i].type        = 0x5;
    idt[i].reserved3   = 0;
    idt[i].dpl         = 0;
    idt[i].present     = 1;
    idt[i].reserved4   = 0;

    // Set the TSS.
    intr_tss[i].esp0   = (unsigned int)kernel_stack;
    intr_tss[i].ss0    = 0x10;
    __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (intr_tss[i].cr3));
    intr_tss[i].eip    = (unsigned int)(isr_array[i]);
    intr_tss[i].eflags = 0x0002;
    intr_tss[i].esp    = (unsigned int)kernel_stack;
    intr_tss[i].cs     = 0x08;
    intr_tss[i].ss     = 0x10;
    intr_tss[i].ds     = 0x10;
    intr_tss[i].es     = 0x10;
    intr_tss[i].fs     = 0x10;
    intr_tss[i].gs     = 0x10;
  }
}

void isr_common(int vector, int error_code) {
  struct tss* prev_tss = get_prev_tss();
  kprintf("Interrupt! vector=%02x code=%08x eip=%08x\n", vector, error_code, prev_tss->eip);
  if (vector == 0xe) {
    unsigned int cr2;
    __asm__ __volatile__ ( "mov %%cr2, %0" : "=r" (cr2));
    kprintf("  at %08x\n", cr2);
    *(char*)(0xB0000000) = '\0';
  }
  __asm__ __volatile__ ( "cli; hlt" );
}
