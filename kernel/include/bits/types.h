#ifndef BITS_TYPES_H
#define BITS_TYPES_H

#include <stdint.h>

typedef int            pid_t;
typedef unsigned short uid_t;
typedef unsigned short gid_t;

typedef short          dev_t;
typedef unsigned short ino_t;
typedef unsigned short nlink_t;
typedef long           off_t;

typedef unsigned int mode_t __attribute__ ((__mode__ (__SI__)));

typedef long blksize_t;
typedef long blkcnt_t;

typedef long time_t;

#endif /* BITS_TYPES_H */
