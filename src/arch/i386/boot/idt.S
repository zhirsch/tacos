#define ASM
#include <asm/interrupt.h>

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

/* Add the .note.GNU-stack section (to tell ld that this object file doesn't
   need an executable stack. */
	.section .note.GNU-stack,"",%progbits
