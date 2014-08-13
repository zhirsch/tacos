#include <asm/multiboot.h>

              // TODO(zhirsch): This should be in the .bss section.
              .data
              .align 0x20
              .skip 0x4000, 0
              .global kernel_stack
kernel_stack:
