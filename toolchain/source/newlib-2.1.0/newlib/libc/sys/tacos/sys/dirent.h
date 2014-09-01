#ifndef _SYS_DIRENT_H_
#define _SYS_DIRENT_H_

#include <sys/types.h>

typedef struct { } DIR;

struct dirent {
  ino_t d_ino;
  char  d_name[];
};

DIR* opendir(const char* name);
struct dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);

#endif /* _SYS_DIRENT_H_ */
