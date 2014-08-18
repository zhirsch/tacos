              .section .data
              .align 0x8
              .global idt
idt:
              .skip 256 * 8, 0

              .global idtp
idtp:
              .word idtp - idt - 1
              .long idt
