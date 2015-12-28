#ifndef ASM_TSS_H
#define ASM_TSS_H

#define ESP0 0x04
#define SS0  0x08

#define DS  0x54
#define ES  0x48
#define FS  0x58
#define GS  0x5c

#define SS  0x50
#define ESP 0x38

#define EFLAGS 0x24

#define CS  0x4c
#define EIP 0x20

#define EAX 0x28
#define ECX 0x2c
#define EDX 0x30
#define EBX 0x34
#define EBP 0x3c
#define ESI 0x40
#define EDI 0x44

#define CR3 0x1c

#endif /* ASM_TSS_H */
