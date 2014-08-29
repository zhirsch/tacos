              .text
              .global switch_to_ring3
switch_to_ring3:
              push  %ebp
              mov   %esp, %ebp

              //    Switch to ring3.  This is done by using iret to "trick" the
              //    processor into thinking that this is returning to a location
              //    in ring3.

              //    Disable interrupts.
              cli

              //    Set the data selectors to the ring3 data descriptor.
              mov   $0x23, %ax
              mov   %ax, %ds
              mov   %ax, %es
              mov   %ax, %fs
              mov   %ax, %gs

              //    Push %ss and %esp onto the stack for ring3.
              push  $0x23
              push  0xc(%ebp)

              // Push a modified version of eflags that has IF set.
              pushf
              pop   %eax
              or    $0x0200, %eax
              push  %eax

              //    Disable NT so that iret doesn't try to return from a task.
              //    Do this after pushing eflags onto the stack above so that
              //    after iret executes, the eflags will have NT set, if
              //    necessary.
              pushf
              pop   %eax
              and   $0xffffbfff, %eax
              push  %eax
              popf

              // Push %cs and %eip onto the stack for ring3.
              push  $0x1b
              mov   0x8(%ebp), %eax
              push %eax

              // Call iret and return to the caller.
              iret
