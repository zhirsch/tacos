#define _INTERRUPT_STUB(x)          \
	.globl intr_ ## x ## _stub ;\
intr_ ## x ## _stub:               ;\
	pusha                      ;\
	call intr_ ## x            ;\
	popa                       ;\
	iret

_INTERRUPT_STUB(divide_error)
_INTERRUPT_STUB(breakpoint)
_INTERRUPT_STUB(overflow)
_INTERRUPT_STUB(bound_range)
_INTERRUPT_STUB(undefined_opcode)
_INTERRUPT_STUB(no_math)
_INTERRUPT_STUB(double_fault)
_INTERRUPT_STUB(invalid_tss)
_INTERRUPT_STUB(segment_not_present)
_INTERRUPT_STUB(general_protection)
_INTERRUPT_STUB(page_fault)
_INTERRUPT_STUB(math_fault)
_INTERRUPT_STUB(alignment_check)
_INTERRUPT_STUB(machine_check)
_INTERRUPT_STUB(simd_exception)

	.section .note.GNU-stack,"",%progbits
