#include <asm/interrupt.h>

              .data
              .align 0x8
              .global gdt
gdt:
              //    NULL
              .long 0x00000000
              .long 0x00000000

              //    RING0 code
              .long 0x0000FFFF
              .long 0x00CF9A00

              //    RING0 data
              .long 0x0000FFFF
              .long 0x00CF9200

              //    RING3 code
              .long 0x0000FFFF
              .long 0x00CFFB00

              //    RING3 data
              .long 0x0000FFFF
              .long 0x00CFF300

              //    TSS for active process.
              .skip 8, 0

              .global gdtp
gdtp:
              .word gdtp - gdt - 1
              .long gdt
