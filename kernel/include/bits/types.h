#ifndef BITS_TYPES_H
#define BITS_TYPES_H

#include <stddef.h>

typedef long int             blkcnt_t;
typedef long int             blksize_t;
typedef unsigned long int    clock_t;
typedef int                  clockid_t;
typedef unsigned long long   dev_t;
typedef unsigned long int    fsblkcnt_t;
typedef unsigned long int    fsfilcnt_t;
typedef unsigned int         gid_t;
typedef unsigned int         id_t;
typedef unsigned long int    ino_t;
typedef int                  key_t;
typedef unsigned int         mode_t;
typedef unsigned long int    nlink_t;
typedef long int             off_t;
typedef int                  pid_t;
typedef long int             suseconds_t;
typedef long int             time_t;
typedef void*                timer_t;
typedef unsigned int         uid_t;
typedef unsigned int         useconds_t;

typedef int                  ssize_t;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

/* These are defined in stddef.h:
typedef size_t;
*/

#endif /* BITS_TYPES_H */
