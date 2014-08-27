#ifndef SYSCALL__EXIT_H
#define SYSCALL__EXIT_H

#include "tss.h"

void syscall__exit(struct tss* prev_tss);

#endif /* SYSCALL__EXIT_H */
