#ifndef STRING_H
#define STRING_H

#include <stddef.h>

size_t strlen(const char* s);
void* memset(void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
char* strncpy(char* dest, const char* src, size_t n);
char* strstr(const char* haystack, const char* needle);

#endif /* STRING_H */
