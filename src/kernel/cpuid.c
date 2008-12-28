/*****************************************************************************
 * cpuid.c
 *****************************************************************************/

#include <tacos/cpuid.h>
#include <tacos/types.h>

/*****************************************************************************
 * cpuid
 *   Issue the cpuid instruction and return the results.
 *****************************************************************************/
void cpuid(uint32_t level, uint32_t *eax, uint32_t *ebx,  uint32_t *ecx, uint32_t *edx)
{
   __asm__ __volatile__ ("cpuid" :
                         "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) :
                         "0" (level));
}
