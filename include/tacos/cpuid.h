/*****************************************************************************
 * cpuid.h
 *****************************************************************************/

#ifndef CPUID_H
#define CPUID_H

#include <tacos/types.h>

/*****************************************************************************
 * cpuid
 *   Issue the cpuid instruction and return the results.
 *****************************************************************************/
void cpuid(uint32_t level, uint32_t *eax, uint32_t *ebx,  uint32_t *ecx, uint32_t *edx);

#endif /* CPUID_H */
