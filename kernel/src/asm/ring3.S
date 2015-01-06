              .text
              .global switch_to_ring3
switch_to_ring3:
              push  %ebp
              mov   %esp, %ebp

              mov   0x10(%ebp), %ax
              mov   %ax, %ds
              mov   %ax, %es
              mov   %ax, %fs
              mov   %ax, %gs

              push  0x10(%ebp)      // ss
              push  0x14(%ebp)      // esp

              pushf
              pop   %eax
              or    0x18(%ebp), %eax
              push  %eax            // eflags

              push  0x8(%ebp)       // cs
              push  0xc(%ebp)       // eip

              //    Set eax to 0, as if this was a function call that returns 0.
              //    This allows this function to be used to return to the child process created by fork.
              mov   $0, %eax

              iret
