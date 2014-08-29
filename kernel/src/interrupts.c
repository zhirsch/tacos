#include "interrupts.h"

#include <stddef.h>
#include <stdint.h>

#include "dt.h"
#include "kprintf.h"
#include "panic.h"
#include "portio.h"
#include "screen.h"

static interrupt_handler_func handlers[256] = { NULL };

static void timer_handler(struct isr_frame* frame) { }

static const char* exceptions[32] = {
  "#DE",  //  0
  "#DB",  //  1
  NULL,   //  2
  "#BP",  //  3
  "#OF",  //  4
  "#BR",  //  5
  "#UD",  //  6
  "#NM",  //  7
  "#DF",  //  8
  NULL,   //  9
  "#TS",  //  a
  "#NP",  //  b
  "#SS",  //  c
  "#GP",  //  d
  "#PF",  //  e
  "#MF",  //  f
  "#AC",  // 10
  "#MC",  // 11
  "#XM",  // 12
  "#VE",  // 13
  NULL,   // 14
  NULL,   // 15
  NULL,   // 16
  NULL,   // 17
  NULL,   // 18
  NULL,   // 19
  NULL,   // 1a
  NULL,   // 1b
  NULL,   // 1c
  NULL,   // 1d
  NULL,   // 1e
  NULL,   // 1f
};

void init_interrupts(void) {
  extern void (*(isr_array[]))(void);
  for (int i = 0; i < 256; i++) {
    idt[i].offset_lo = (((uintptr_t)isr_array[i]) & 0x0000FFFF) >> 0;
    idt[i].selector  = 0x08;
    idt[i].reserved  = 0;
    idt[i].type      = 0xE;  // Interrupt gate.
    idt[i].dpl       = 0;
    idt[i].present   = 1;
    idt[i].offset_hi = (((uintptr_t)isr_array[i]) & 0xFFFF0000) >> 16;
  }

  // TODO(zhirsch): Move this somewhere specific to the PIT.
  interrupt_register_handler(0x20, timer_handler);
}

void interrupt_register_handler(int vector, interrupt_handler_func func) {
  if (handlers[vector] != NULL) {
    panic("Interrupt handler already registered for vector %d: %08lx\n",
          vector, (uintptr_t)handlers[vector]);
  }
  handlers[vector] = func;
}

void isr_common(struct isr_frame* frame) {
  if (handlers[frame->vector] != NULL) {
    handlers[frame->vector](frame);
  } else {
    kprintf("Interrupt! vector=%02x code=%08x\n", frame->vector, frame->error_code);
    print_call_stack(frame->eip, frame->ebp);
    panic("  unhandled (%s)!\n", (exceptions[frame->vector] == NULL) ? "?" : exceptions[frame->vector]);
  }

  // EOI
  if (0x20 <= frame->vector && frame->vector < 0x30) {
    if (0x28 <= frame->vector && frame->vector < 0x30) {
      outb(0x20+0x80, 0x20);
    }
    outb(0x20, 0x20);
  }
}
