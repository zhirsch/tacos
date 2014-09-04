#ifndef BITS_DIRENT_H
#define BITS_DIRENT_H

#include <sys/types.h>

typedef struct { } DIR;

struct dirent {
  ino_t d_ino;
  char  d_name[];
};

#endif /* BITS_DIRENT_H */
