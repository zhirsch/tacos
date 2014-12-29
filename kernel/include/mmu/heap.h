#ifndef MMU_HEAP_H
#define MMU_HEAP_H

#include <stddef.h>

void* kmalloc(size_t size);
void* kcalloc(size_t nmemb, size_t size);
void kfree(void* ptr);

#endif /* MMU_HEAP_H */
