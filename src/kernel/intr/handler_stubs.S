        .globl intr_divide_error_stub
intr_divide_error_stub:
        pusha
        call intr_divide_error
        popa
        iret

        .globl intr_breakpoint_stub
intr_breakpoint_stub:
        pusha
        call intr_breakpoint
        popa
        iret

        .globl intr_overflow_stub
intr_overflow_stub:
        pusha
        call intr_overflow
        popa
        iret

        .globl intr_bound_range_stub
intr_bound_range_stub:
        pusha
        call intr_bound_range
        popa
        iret

        .globl intr_undefined_opcode_stub
intr_undefined_opcode_stub:
        pusha
        call intr_undefined_opcode
        popa
        iret

        .globl intr_no_math_stub
intr_no_math_stub:
        pusha
        call intr_no_math
        popa
        iret

        .globl intr_double_fault_stub
intr_double_fault_stub:
        pusha
        call intr_double_fault
        popa
        iret

        .globl intr_invalid_tss_stub
intr_invalid_tss_stub:
        pusha
        call intr_invalid_tss
        popa
        iret

        .globl intr_segment_not_present_stub
intr_segment_not_present_stub:
        pusha
        call intr_segment_not_present
        popa
        iret

        .globl intr_general_protection_stub
intr_general_protection_stub:
        pusha
        call intr_general_protection
        popa
        iret

        .globl intr_page_fault_stub
intr_page_fault_stub:
        pusha
        call intr_page_fault
        popa
        iret

        .globl intr_math_fault_stub
intr_math_fault_stub:
        pusha
        call intr_math_fault
        popa
        iret

        .globl intr_alignment_check_stub
intr_alignment_check_stub:
        pusha
        call intr_alignment_check
        popa
        iret

        .globl intr_machine_check_stub
intr_machine_check_stub:
        pusha
        call intr_machine_check
        popa
        iret

        .globl intr_simd_exception_stub
intr_simd_exception_stub:
        pusha
        call intr_simd_exception
        popa
        iret


#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
