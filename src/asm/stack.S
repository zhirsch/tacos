#include <asm/multiboot.h>

        .bss
        .align  0x20
        .skip   0x4000, 0
        .global kernel_stack
kernel_stack:
