#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

#include <bits/stat.h>
#include <sys/types.h>

int stat(const char* path, struct stat* buf);
int fstat(int fd, struct stat* buf);
int lstat(const char* path, struct stat* buf);
mode_t umask(mode_t mask);

#endif /* _SYS_STAT_H_ */
