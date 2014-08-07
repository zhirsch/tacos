#ifndef ASM_INTERRUPT_H
#define ASM_INTERRUPT_H

/* The number of interrupts that are accounted for. */
#define NUM_INTERRUPTS 32

/* Convinient names for common interrupts. */
#define INTR_DE 0
#define INTR_BP 3
#define INTR_OF 4
#define INTR_BR 5
#define INTR_UD 6
#define INTR_NM 7
#define INTR_DF 8
#define INTR_TS 10
#define INTR_NP 11
#define INTR_SS 12
#define INTR_GP 13
#define INTR_PF 14
#define INTR_MF 16
#define INTR_AC 17
#define INTR_MC 18
#define INTR_XF 19

#endif /* ASM_INTERRUPT_H */
