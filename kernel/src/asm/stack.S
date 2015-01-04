              .bss
              .align 0x1000
              .global kernel_stack_end
kernel_stack_end:
              .skip 0x4000
              .global kernel_stack_start
kernel_stack_start:

              .size kernel_stack_end, kernel_stack_start - kernel_stack_end
