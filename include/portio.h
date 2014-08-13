#ifndef PORTIO_H
#define PORTIO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ __volatile__ ("outb %0, %1" : : "a" (val), "dN" (port));
}

static inline void outw(uint16_t port, uint16_t val) {
  __asm__ __volatile__ ("outw %0, %1" : : "a" (val), "dN" (port));
}

static inline void outsw(uint16_t port, const uint16_t *addr, int cnt) {
  __asm__ __volatile__ ("rep outsw" : "+S" (addr), "+c" (cnt) : "dN" (port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t val;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (val) : "dN" (port) : "memory");
  return val;
}

static inline void insw(uint16_t port, uint16_t* buffer, int cnt) {
  __asm__ __volatile__ ("rep insw" : : "c" (cnt), "dN" (port), "D" (buffer) : "memory");
}

#endif /* PORTIO_H */
