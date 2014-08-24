              .text
              .global _start
_start:
              //    Multiboot starts execution here with %eip set to a physical address.
              //    The first thing is to configure virtual memory.
              mov   $kernel_pagedir - 0xC0000000, %ecx
              mov   %ecx, %cr3
              //    Set PSE.
              mov   %cr4, %ecx
              or    $0x00000010, %ecx
              mov   %ecx, %cr4
              //    Set PG and WP.
              mov   %cr0, %ecx
              or    $0x80010000, %ecx
              mov   %ecx, %cr0
              //    Indirect jump so that %eip is set to a virtual address.
              lea   .L1, %ecx
              jmp   *%ecx

.L1:
              //    Virtual memory is now configured.
              //    Load the global and interrupt descriptor tables.
              lgdt  (gdtp)
              lidt  (idtp)
              //    Set the data segments.
              mov   $0x10, %cx      // RING0
              mov   %cx, %ds
              mov   %cx, %es
              mov   %cx, %fs
              mov   %cx, %gs
              mov   %cx, %ss
              //    Set the code segment.
              ljmp  $0x08, $.L2       // RING0

.L2:
              //    Set the stack.
              movl  $kernel_stack_top, %esp
              //    Reset EFLAGS.
              pushl $0
              popf

              //    Set ebp to a sentinel so that stack tracing can stop.
              mov   $0, %ebp

              //    Call kmain.
              pushl %ebx            // multiboot struct
              pushl %eax            // multiboot magic value
              call  kmain


              // Define the multiboot header.
#include <asm/multiboot.h>
              .align 4
multiboot_header:
              .long MULTIBOOT_HEADER_MAGIC
              .long MULTIBOOT_HEADER_FLAGS
              .long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
