#include <stddef.h>
#include <stdint.h>

void* memset(void* s, int c, size_t n) {
  for (size_t i = 0; i < n; i++) {
    *(((char*)s) + i) = (char)c;
  }
  return s;
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

void* memcpy(void* dest, const void* src, size_t n) {
  return memmove(dest, src, n);
}

size_t strlen(const char* s) {
  size_t c = 0;
  while (*(s++) != '\0') {
    c++;
  }
  return c;
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
