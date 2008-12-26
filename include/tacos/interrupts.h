/*****************************************************************************
 * interrupts.h
 *****************************************************************************/

#ifndef TACOS_INTERRUPTS_H
#define TACOS_INTERRUPTS_H

#include <asm/interrupts.h>
#include <tacos/types.h>

typedef void (*intr_handler_t)(void);

extern void prepare_interrupt_handler(uint8_t intr, intr_handler_t handler);

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
