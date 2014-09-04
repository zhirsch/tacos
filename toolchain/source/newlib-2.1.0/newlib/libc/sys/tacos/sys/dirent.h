#ifndef _SYS_DIRENT_H_
#define _SYS_DIRENT_H_

#include <bits/dirent.h>

DIR* opendir(const char* name);
struct dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);

#endif /* _SYS_DIRENT_H_ */
