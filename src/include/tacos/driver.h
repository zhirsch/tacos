/*****************************************************************************
 * driver.h
 *****************************************************************************/

#ifndef _TACOS_DRIVER_H
#define _TACOS_DRIVER_H

#include <tacos/kernel.h>
#include <tacos/types.h>

typedef struct
{
      const char * const name;
      const version_t    version;
      const uintptr_t    entry_point;
      const uintptr_t    stack;
} driver_info_t;

#endif /* _TACOS_DRIVER_H */
