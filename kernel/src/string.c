#include "string.h"

#include <stddef.h>
#include <stdint.h>

#include "kmalloc.h"

void* memcpy(void* dest, const void* src, size_t n) {
  return memmove(dest, src, n);
}

void* memmove(void* dest, const void* src, size_t n) {
  if ((uintptr_t)src < (uintptr_t)dest) {
    // Copy from the end.
    for (size_t i = n; i > 0; i--) {
      *(((char*)dest) + i - 1) = *(((char*)src) + i - 1);
    }
  } else {
    // Copy from the beginning.
    for (size_t i = 0; i < n; i++) {
      *(((char*)dest) + i) = *(((char*)src) + i);
    }
  }
  return dest;
}

void* memset(void* s, int c, size_t n) {
  for (size_t i = 0; i < n; i++) {
    *(((char*)s) + i) = (char)c;
  }
  return s;
}

char* strcpy(char* dest, const char* src) {
  char* t = dest;
  while (*src != '\0') {
    *(dest++) = *(src++);
  }
  return t;
}

char* strdup(const char* s) {
  char* t = kmalloc(strlen(s) + 1);
  strcpy(t, s);
  t[strlen(s)] = '\0';
  return t;
}

int strcmp(const char* s1, const char* s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 != *s2) {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return 0;
}

char* strncpy(char* dest, const char* src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  for (; i < n; i++) {
    dest[i] = '\0';
  }
  return dest;
}

// TODO(zhirsch): Don't implement this recursively.
char* strstr(const char* haystack, const char* needle) {
  char* p;
  if (*needle == '\0') {
    return (char*)haystack;
  }
  if (*haystack == '\0') {
    return NULL;
  }
  if (*haystack != *needle) {
    return strstr(haystack+1, needle);
  }
  p = strstr(haystack+1, needle+1);
  if (p == NULL) {
    return strstr(haystack+1, needle);
  }
  return (char*)haystack;
}

size_t strlen(const char* s) {
  size_t c = 0;
  while (*(s++) != '\0') {
    c++;
  }
  return c;
}

char* strsep(char** stringp, const char* delim) {
  char* const start = *stringp;
  if (*stringp == NULL) {
    return NULL;
  }

  for (char* p = *stringp; *p != '\0'; p++) {
    for (const char* d = delim; *d != '\0'; d++) {
      if (*p == *d) {
        *p = '\0';
        *stringp = p + 1;
        return start;
      }
    }
  }

  *stringp = NULL;
  return start;
}
