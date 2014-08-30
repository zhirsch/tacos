#ifndef SBRK_H
#define SBRK_H

#include <stdint.h>

void sbrk_grow(uintptr_t* cpb, uintptr_t increment);
void sbrk_shrink(uintptr_t* cpb, uintptr_t decrement);

#endif /* SBRK_H */
