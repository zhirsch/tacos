#include "fpu.h"

void init_fpu(void) {
  fpu_reset();
  __asm__ __volatile__ ("fninit");
}

void fpu_reset(void) {
  __asm__ __volatile__ ("clts");
}
