#ifndef	SYS_FCNTL_H
#define	SYS_FCNTL_H

#include <bits/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>		/* sigh. for the mode bits for open/creat */

int open(const char* path, int flags, ...);
int creat(const char* path, mode_t mode);
int fcntl(int fd, int cmd, ...);

#endif	/* SYS_FCNTL_H */
