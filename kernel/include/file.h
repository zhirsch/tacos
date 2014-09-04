#ifndef FILE_H
#define FILE_H

#include "bits/types.h"

struct file {
  const char* path;
  ino_t inode;
};

#endif /* FILE_H */
