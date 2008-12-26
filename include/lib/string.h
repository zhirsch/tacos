/*****************************************************************************
 * string.h
 *****************************************************************************/

#ifndef STRING_H
#define STRING_H

#include <tacos/types.h>

char *strncpy(char *dest, const char *src, size_t n);

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memsetw(void *s, int c, size_t n);
size_t strlen(const char *str);

#endif /* STRING_H */
