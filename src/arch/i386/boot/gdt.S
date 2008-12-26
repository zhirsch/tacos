#define ASM
#include <asm/process.h>

/* --------------------------- gdt ----------------------------------------- */
        .data
        .align  0x8
        .globl  gdt
gdt:
        .long   0x00000000      /* The NULL descriptor */
        .long   0x00000000

        .long   0x0000FFFF      /* A RING0 code segment */
        .long   0x00CF9A00

        .long   0x0000FFFF      /* A RING0 data segment */
        .long   0x00CF9200

        .long   0x0000FFFF      /* A RING3 code segment */
        .long   0x00CFFB00

        .long   0x0000FFFF      /* A RING3 data segment */
        .long   0x00CFF300

        .skip   NUM_PROCESSES * 8, 0  /* Create room for the processes */
        .skip   NUM_INTERRUPTS * 8, 0 /* Create root for the interrupts */

	.globl gdtp
gdtp:
        .word   gdtp - gdt - 1
        .long   gdt

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
