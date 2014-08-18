#include "interrupts.h"

#include <stdlib.h>

#include "dt.h"
#include "kprintf.h"
#include "portio.h"
#include "screen.h"
#include "tss.h"

static char intr_stack[0x4000] __attribute__((aligned(0x20)));
static struct tss intr_tss[256] __attribute__((aligned(0x1000)));
static interrupt_handler_func handlers[256] = { NULL };

extern void (*(isr_array[]))(void);

static void timer_handler(int vector, int error_code, struct tss* prev_tss) { }

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
    intr_tss[i].esp0   = (unsigned int)intr_stack;
    intr_tss[i].ss0    = 0x10;
    __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (intr_tss[i].cr3));
    intr_tss[i].eip    = (unsigned int)(isr_array[i]);
    intr_tss[i].eflags = 0x0002;
    intr_tss[i].esp    = (unsigned int)intr_stack;
    intr_tss[i].cs     = 0x08;
    intr_tss[i].ss     = 0x10;
    intr_tss[i].ds     = 0x10;
    intr_tss[i].es     = 0x10;
    intr_tss[i].fs     = 0x10;
    intr_tss[i].gs     = 0x10;
  }

  // TODO(zhirsch): Move this somewhere specific to the PIT.
  interrupt_register_handler(0x20, timer_handler);
}

void interrupt_register_handler(int vector, interrupt_handler_func func) {
  if (handlers[vector] != NULL) {
    kprintf("Interrupt handler already registered for vector %d: %08lx\n",
            vector, (uintptr_t)handlers[vector]);
    __asm__ __volatile__ ("cli;hlt");
  }
  handlers[vector] = func;
}

void isr_common(int vector, int error_code) {
  struct tss* prev_tss = get_prev_tss();

  if (handlers[vector] != NULL) {
    handlers[vector](vector, error_code, prev_tss);
  } else {
    kprintf("Interrupt! vector=%02x code=%08x eip=%08x\n", vector, error_code, prev_tss->eip);
    puts("  unhandled!\n");
  }

  // EOI
  if (0x20 <= vector && vector < 0x30) {
    if (0x28 <= vector && vector < 0x30) {
      outb(0x20+0x80, 0x20);
    }
    outb(0x20, 0x20);
  }
}
