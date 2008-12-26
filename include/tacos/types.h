/*****************************************************************************
 * types.h
 *****************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef _Bool              bit_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef char               int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;

#if __WORDSIZE == 64
typedef long               intptr_t;
typedef unsigned long      uintptr_t;
#else
typedef int                intptr_t;
typedef unsigned int       uintptr_t;
#endif

typedef int32_t time_t;
typedef uint32_t clock_t;

/* Type to contain a unique process identifier.
 *  This is the index into the proc table for the process, unless it's set to
 *  INVALID_PID, which is the pid for no process.
 */
typedef int16_t pid_t;

#ifndef INLINE
# define INLINE inline __attribute__ ((always_inline))
#endif

#endif /* TYPES_H */
