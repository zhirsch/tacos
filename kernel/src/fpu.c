#include "fpu.h"

#include <stdint.h>

#include "interrupts.h"

static void no_device_available_handler(struct isr_frame* frame);

void init_fpu(void) {
  fpu_reset();
  __asm__ __volatile__ ("fninit");
  interrupt_register_handler(0x7, no_device_available_handler);
}

void fpu_reset(void) {
  __asm__ __volatile__ ("clts");
}

static void no_device_available_handler(struct isr_frame* frame) {
  // TODO(zhirsch): Save and restore the FPU state.
  fpu_reset();
}
