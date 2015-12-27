#include "asm/tss.h"

        .text
        .global process_start
        // void process_start(struct tss* tss)
process_start:
        push  %ebp
        mov   %esp, %ebp

        // arg1 = tss
        mov   0x8(%ebp), %edx

        // Set all the segment registers.
        mov   DS(%edx), %ax
        mov   %ax, %ds
        mov   ES(%edx), %ax
        mov   %ax, %es
        mov   FS(%edx), %ax
        mov   %ax, %fs
        mov   GS(%edx), %ax
        mov   %ax, %gs

        // Set up the stack so that iret will jump to the user code in ring 3.
        // The stack needs to look like:
        //
        //   ss
        //   esp
        //   eflags
        //   cs
        //   eip

        push  SS(%edx)
        push  ESP(%edx)

        // Set the eflags based on their current values.
        pushf
        pop   %eax
        or    EFLAGS(%edx), %eax
        push  %eax

        push  CS(%edx)
        push  EIP(%edx)

        // Restore the general purpose register values.
        push  EAX(%edx)
        push  ECX(%edx)
        push  EDX(%edx)
        push  EBX(%edx)
        push  ESP(%edx)
        push  EBP(%edx)
        push  ESI(%edx)
        push  EDI(%edx)
        popa

        // Set eax to 0 for the return value from fork.
        mov   $0, %eax

        iret
