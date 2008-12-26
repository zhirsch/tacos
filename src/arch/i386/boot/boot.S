#define ASM
#include <multiboot.h>
#include <asm/mmu.h>
#include <asm/process.h>
#include <asm/interrupts.h>

        .text

        .globl  _start
_start:
        /* When GRUB passes execution to here, instructions are being fetched
             from the first 4 MB in _physical_ memory. But ld links the
             kernel so that the VMAs are in the last GB of memory. So we need
             to play tricks with paging. */

        /* Load the address of the kernel_pagedir into the PDBR --
             Need to play tricks since VMA(kernel_pagedir) = 0xC01xxxxx
             But PMA(kernel_pagedir) = 0x001xxxxx and %cr3 needs the physical
             address of the page directory. */
        mov     $kernel_pagedir - KERNEL_VIRTUAL_BASE, %ecx
        mov     %ecx, %cr3

        /* Set the PSE bit in %cr4 to enable 4MB pages. */
        mov     %cr4, %ecx
        or      $0x00000010, %ecx
        mov     %ecx, %cr4

        /* Set the PG bit in %cr0 to enable paging. */
        mov     %cr0, %ecx
        or      $0x80000000, %ecx
        mov     %ecx, %cr0

        /* Now paging will translate addresses in the first 4 MB of the last GB
             of memory to the first 4 MB of the first GB in memory (which is
             where the kernel is physically located). */

        /* Jump to the actual entry point in kernel space. */
        lea     boot__entry, %ecx
        jmp     *%ecx

/*****************************************************************************
 * The following is the multiboot header -- GRUB needs it -- so don't touch.
 *****************************************************************************/
        .align  4
multiboot_header:
        .long   MULTIBOOT_HEADER_MAGIC
        .long   MULTIBOOT_HEADER_FLAGS
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

/*****************************************************************************
 * boot__entry
 *****************************************************************************/
boot__entry:
        /* Load the global descriptor table pointer */
        lgdt    (gdtp)
        /* Load the interrupt descriptor table pointer */
        lidt    (idtp)

        /* Set up the selectors for RING0 */
        mov     $0x10, %cx      /* RING0 data selector */
        mov     %cx, %ds
        mov     %cx, %es
        mov     %cx, %fs
        mov     %cx, %gs
        mov     %cx, %ss
        ljmp    $0x08, $tmp     /* RING0 code selector */
tmp:
        /* Set the kernel's stack */
        movl    $kstktop, %esp

        /* Reset the EFLAGS */
        pushl   $0
        popf

        /* Call the kmain function to continue system setup. */
        pushl   %ebx            /* multiboot information structure. */
        pushl   %eax            /* multiboot magic value. */
        call    kmain

/* =========================== CODE ENDS HERE ============================== */

/* --------------------------- stack --------------------------------------- */
        .bss
        .align  0x20
/* allocate space for the kernel's stack */
kstack:
        .space  STACK_SIZE
        .globl  kstktop
kstktop:

/* --------------------------- kernel_pagedir ------------------------------ */
        .data
        .align  0x1000
        .globl  kernel_pagedir
kernel_pagedir:
        /* The first 4 MB are mapped 1:1 using a 4 MB page */
        .long   0x00000083

        /* Create zeroed entries until the one for the kernel */
        .skip   (KERNEL_PAGE_NUMBER - 1) * 4, 0

        /* Create the kernel's page directory entry -- 4 MB page.
             Maps 0xC0000000-0xC0400000 onto
                  0x00000000-0x00400000. */
        .long   0x00000083

        /* Fill in the rest of the page directory */
        .skip   (1024 - KERNEL_PAGE_NUMBER - 1) * 4, 0

/* --------------------------- gdt ----------------------------------------- */
        .data
        .align  0x8
        .globl  gdt
gdt:
        .long   0x00000000      /* The NULL descriptor */
        .long   0x00000000

        .long   0x0000FFFF      /* A RING0 code segment */
        .long   0x00CF9A00

        .long   0x0000FFFF      /* A RING0 data segment */
        .long   0x00CF9200

        .long   0x0000FFFF      /* A RING3 code segment */
        .long   0x00CFFB00

        .long   0x0000FFFF      /* A RING3 data segment */
        .long   0x00CFF300

        .skip   NUM_PROCESSES * 8, 0  /* Create room for the processes */
        .skip   NUM_INTERRUPTS * 8, 0 /* Create root for the interrupts */
gdtp:
        .word   gdtp - gdt - 1
        .long   gdt

/* --------------------------- idt ----------------------------------------- */
        .data
        .align  0x8
        .globl  idt
idt:
        .skip   NUM_INTERRUPTS * 8, 0      /* There are a max of 256 entries in an IDT */
idtp:
        .word   idtp - idt - 1
        .long   idt

/* --------------------------- tss ----------------------------------------- */
        .data
        .align  0x8
        .globl  tss
tss:
        /* Create enough entries in the tss for the interrupts and processes */
        /*  (each is 104 bytes long) */
        .skip   NUM_PROCESSES * 104, 0
        .skip   NUM_INTERRUPTS * 104, 0
tssend:
	
#if defined(__linux__) && defined(__ELF__)
	.section .note.GNU-stack,"",%progbits
#endif
