#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);

char* strcpy(char* dest, const char* src);

char* strdup(const char* s);

int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

char* strncpy(char* dest, const char* src, size_t n);

char* strstr(const char* haystack, const char* needle);

size_t strlen(const char* s);

char* strsep(char** stringp, const char* delim);

#endif /* STRING_H */
