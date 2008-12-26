#define ASM
#include <asm/process.h>

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
	
#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
