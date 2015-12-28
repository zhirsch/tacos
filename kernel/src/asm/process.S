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

        // Set ss0 and esp0 for the process so that the appropriate kernel stack
        // will be used when the process is interrupted.
        mov   ESP0(%edx), %eax
        mov   %eax, (ESP0 + g_tss)
        mov   SS0(%edx), %ax
        mov   %ax, (SS0 + g_tss)

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


        .text
        .global process_switch
        // void process_switch(struct tss* old_tss, struct tss* new_tss)
process_switch:
        push    %ebp
        mov     %esp, %ebp

        // arg1 = old_tss
        mov     0x8(%ebp), %eax
        // arg2 = new_tss
        mov     0xc(%ebp), %edx

        // Save the old process's state.
        pusha
        push    current_process

        // Save the old esp0 and restore the new esp0.
        mov     %esp, ESP0(%eax)
        mov     ESP0(%edx), %esp

        // Switch to the new page directory.
        mov     CR3(%edx), %eax
        mov     %eax, %cr3

        // Set ss0 and esp0 for the new process so that the appropriate kernel
        // stack will be used when the process is interrupted.
        mov     ESP0(%edx), %eax
        mov     %eax, (ESP0 + g_tss)
        mov     SS0(%edx), %ax
        mov     %ax, (SS0 + g_tss)

        // Restore the new process's state.
        pop     current_process
        popa

        // Return to the new process.
        pop     %ebp
        ret
