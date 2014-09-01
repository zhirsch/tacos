              .global _start
_start:
              //    Clear frame pointer.
              mov   $0x0, %ebp

              //    Set the environ pointer.
              mov   0x8(%esp), %eax
              mov   %eax, (environ)

              //    argc, argv, envp are already on the stack.
              call  main

              push  %eax
              call  exit

1:            hlt
              jmp   1b
