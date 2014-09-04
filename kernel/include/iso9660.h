#ifndef ISO9660_H
#define ISO9660_H

#include <stddef.h>

#include "bits/stat.h"
#include "bits/types.h"

#include "file.h"

int iso9660_open(const char* path, int flags, struct file* file);
int iso9660_fstat(struct file* file, struct stat* st);
int iso9660_pread(struct file* file, void* buf, size_t count, off_t offset);
int iso9660_close(struct file* file);

#endif /* ISO9960_H */
