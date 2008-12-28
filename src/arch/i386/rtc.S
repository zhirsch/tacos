#define ENSURE_NOT_UPDATING()   \
0:      mov     $10, %al       ;\
        outb    %al, $0x70     ;\
        inb     $0x71, %al     ;\
        test    $0x80, %al     ;\
        jne     0b

        .text
        .global getsecond
getsecond:
        ENSURE_NOT_UPDATING()
        mov     $0x0, %al
        outb    %al, $0x70
        xor     %eax, %eax
        inb     $0x71, %al
        ret

        .global gethour
gethour:
        ENSURE_NOT_UPDATING()
        mov     $0x04, %al
        outb    %al, $0x70
        xor     %eax, %eax
        inb     $0x71, %al
        ret

#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
