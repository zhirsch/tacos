#ifndef BITS_STAT_H
#define BITS_STAT_H

#include "bits/types.h"

struct stat {
  dev_t     st_dev;
  ino_t     st_ino;
  mode_t    st_mode;
  nlink_t   st_nlink;
  uid_t     st_uid;
  gid_t     st_gid;
  dev_t     st_rdev;
  off_t     st_size;
  blksize_t st_blksize;
  blkcnt_t  st_blocks;
  time_t    st_atime;
  time_t    st_mtime;
  time_t    st_ctime;
};

#define _IFMT   0170000
#define _IFDIR  0040000
#define _IFCHR  0020000
#define _IFBLK  0060000
#define _IFREG  0100000
#define _IFLNK  0120000
#define _IFSOCK 0140000
#define _IFIFO  0010000

#define S_IFMT  _IFMT
#define S_IFDIR _IFDIR
#define S_IFCHR _IFCHR
#define S_IFREG _IFREG

#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_IXUSR 0000100
#define S_IXGRP 0000010
#define S_IXOTH 0000001

#define S_IWUSR 0000200
#define S_IWGRP 0000020
#define S_IWOTH 0000002

#define S_IRUSR 0000400
#define S_IRGRP 0000040
#define S_IROTH 0000004

#define S_ISBLK(m)  (((m)&_IFMT) == _IFBLK)
#define S_ISCHR(m)  (((m)&_IFMT) == _IFCHR)
#define S_ISDIR(m)  (((m)&_IFMT) == _IFDIR)
#define S_ISFIFO(m) (((m)&_IFMT) == _IFIFO)
#define S_ISREG(m)  (((m)&_IFMT) == _IFREG)
#define S_ISLNK(m)  (((m)&_IFMT) == _IFLNK)
#define S_ISSOCK(m) (((m)&_IFMT) == _IFSOCK)

#endif /* BITS_STAT_H */
