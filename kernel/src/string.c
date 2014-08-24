#include <string.h>

#include <stdint.h>

void* memset(void* s, int c, size_t n) {
  for (size_t i = 0; i < n; i++) {
    *(((char*)s) + i) = (char)c;
  }
  return s;
}

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

size_t strlen(const char* s) {
  size_t c = 0;
  while (*(s++) != '\0') {
    c++;
  }
  return c;
}
