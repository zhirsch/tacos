/*****************************************************************************
 * kernel.h
 *****************************************************************************/

#ifndef KERNEL_H
#define KERNEL_H

#include <tacos/types.h>

typedef struct
{
   uint8_t major;
   uint8_t minor;
   uint8_t tiny;
} version_t;

extern version_t kversion;

static INLINE
uint32_t memory_get_cr3(void)
{
   uint32_t cr3;
   __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (cr3));
   return cr3;
}

#endif /* KERNEL_H */
