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

#endif /* KERNEL_H */
