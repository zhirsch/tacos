#define ASM
#include <asm/process.h>
#include <asm/interrupt.h>

/* --------------------------- tss ----------------------------------------- */
        .data
        .align  0x8
        .globl  tss
tss:
        /* Create enough entries in the tss for the interrupts and processes */
        /*  (each is 104 bytes long) */
        .skip   NUM_PROCESSES * 104, 0
        .skip   NUM_INTERRUPTS * 104, 0
tssend:

/* Add the .note.GNU-stack section (to tell ld that this object file doesn't
   need an executable stack. */
//	.section .note.GNU-stack,"",%progbits
