#define ASM
#include <asm/interrupts.h>

/* --------------------------- idt ----------------------------------------- */
        .data
        .align  0x8
        .globl  idt
idt:
        .skip   NUM_INTERRUPTS * 8, 0 /* The max is 256 entries in an IDT */

	.globl idtp
idtp:
        .word   idtp - idt - 1
        .long   idt

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
