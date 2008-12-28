/*****************************************************************************
 * memory.h
 *****************************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <tacos/types.h>

/**
 * Memory_GetPageTableBaseAddress
 *
 * Get the value of the PGTR (cr3).
 */
static INLINE uint32_t Memory_GetPageTableBaseAddress(void)
{
   uint32_t cr3;
   __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (cr3));
   return cr3;
}

#endif /* MEMORY_H */
