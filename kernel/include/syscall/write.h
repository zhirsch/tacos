#ifndef SYSCALL_WRITE_H
#define SYSCALL_WRITE_H

#include "tss.h"

void syscall_write(struct tss* prev_tss);

#endif /* SYSCALL_WRITE_H */
