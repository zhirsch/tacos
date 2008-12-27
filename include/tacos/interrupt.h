/*****************************************************************************
 * interrupts.h
 *****************************************************************************/

#ifndef TACOS_INTERRUPTS_H
#define TACOS_INTERRUPTS_H

#include <asm/interrupt.h>
#include <tacos/types.h>

/* Create a selector into the GDT for an interrupt's task. */
#define INTERRUPT_CreateSelector(x, y) \
   GDT_CreateSelector(GDT_TASK_BASE + NUM_PROCESSES + (x), (y))

typedef void (*interrupt_handler_t)(void);

/*****************************************************************************
 * Interrupt_AssignNewHandler
 *   Assign a new handler for a specific interrupt. Any existing handler for
 *   the interrupt is removed.
 *****************************************************************************/
extern void Interrupt_AssignNewHandler(uint8_t intr, interrupt_handler_t hdlr);

/* Stubs for handling interrupts. */
/* XXX: These should be moved elsewhere. */
extern void intr_divide_error_stub(void);
extern void intr_breakpoint_stub(void);
extern void intr_overflow_stub(void);
extern void intr_bound_range_stub(void);
extern void intr_undefined_opcode_stub(void);
extern void intr_no_math_stub(void);
extern void intr_double_fault_stub(void);
extern void intr_invalid_tss_stub(void);
extern void intr_segment_not_present_stub(void);
extern void intr_general_protection_stub(void);
extern void intr_page_fault_stub(void);
extern void intr_math_fault_stub(void);
extern void intr_alignment_check_stub(void);
extern void intr_machine_check_stub(void);
extern void intr_simd_exception_stub(void);

#endif /* TACOS_PROCESS_H */
