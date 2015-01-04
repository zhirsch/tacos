/* Define the kernel page directory. */
              .data
              .align 0x1000
              .global kernel_pagedir
kernel_pagedir:
              .long 0x00000083   // The first 4 MiB are identity mapped.
              .skip 1023 * 4, 0  // The rest will be filled in later.
              .size kernel_pagedir, . - kernel_pagedir

/* A macro for populating a page table. */
              .macro define_pagetable base, i=0
              .if \i < 64
              .long \base | ((\i * 16 +  0) << 12) | 0x3
              .long \base | ((\i * 16 +  1) << 12) | 0x3
              .long \base | ((\i * 16 +  2) << 12) | 0x3
              .long \base | ((\i * 16 +  3) << 12) | 0x3
              .long \base | ((\i * 16 +  4) << 12) | 0x3
              .long \base | ((\i * 16 +  5) << 12) | 0x3
              .long \base | ((\i * 16 +  6) << 12) | 0x3
              .long \base | ((\i * 16 +  7) << 12) | 0x3
              .long \base | ((\i * 16 +  8) << 12) | 0x3
              .long \base | ((\i * 16 +  9) << 12) | 0x3
              .long \base | ((\i * 16 + 10) << 12) | 0x3
              .long \base | ((\i * 16 + 11) << 12) | 0x3
              .long \base | ((\i * 16 + 12) << 12) | 0x3
              .long \base | ((\i * 16 + 13) << 12) | 0x3
              .long \base | ((\i * 16 + 14) << 12) | 0x3
              .long \base | ((\i * 16 + 15) << 12) | 0x3
              define_pagetable \base, "(\i+1)"
              .endif
              .endm

/* Define the kernel page tables. */
kernel_pagetables:
              //    Map the first 8 MiB of kernel memory to the first 8 MiB on the system.
              define_pagetable 0x00000000
              define_pagetable 0x00400000
              //    Everything else is unmapped.
              .skip 253 * 4096, 0
              .size kernel_pagetables, . - kernel_pagetables

/* Fills in the page table addresses in the page directory. */
              .text
              .align 0x1000
              .global init_kernel_pagedir
init_kernel_pagedir:
              push  %ebp
              mov   %esp, %ebp

              //    Load the physical address of the kernel page directory.
              //    Since paging isn't enabled yet, the physical address needs to be written to.
              mov   $kernel_pagedir, %edx
              sub   $0xC0000000, %edx

              //    Load the physical address of the kernel page tables.
              mov   $kernel_pagetables, %eax
              sub   $0xC0000000, %eax
              or    $0x00000003, %eax

              //    Set each page table address.
              mov   $255, %ecx      // 255 entries to set,
              add   $0xC00, %edx    //   starting at the last GiB.
1:            mov   %eax, (%edx)
              add   $4, %edx
              add   $0x1000, %eax
              loop  1b

              //    Map the last PDE to the page directory itself.
              mov   $kernel_pagedir, %eax
              sub   $0xC0000000, %eax
              or    $0x00000003, %eax
              mov   %eax, (%edx)

              pop   %ebp
              ret
