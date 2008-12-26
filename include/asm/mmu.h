/*****************************************************************************
 * asm/mmu.h
 *****************************************************************************/

/* The base address of the kernel in virtual memory. */
#define KERNEL_VIRTUAL_BASE 0xC0000000
/* The kernel's page number (index into the kernel's page directory). */
#define KERNEL_PAGE_NUMBER (KERNEL_VIRTUAL_BASE >> 22)
