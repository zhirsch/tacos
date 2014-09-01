#include "pic.h"

#include <stdint.h>

#include "portio.h"

static inline void send_command(uint8_t command, int pic);
static inline void send_data(uint8_t data, int pic);
static inline uint8_t recv_data(int pic);

void init_pic(uint8_t base0, uint8_t base1) {
  // ICW1 (ICW4 + INIT)
  send_command(0x01 | 0x10, 0);
  send_command(0x01 | 0x10, 1);

  // ICW2 (vector offsets)
  send_data(base0, 0);
  send_data(base1, 1);

  // ICW3 (cascade)
  send_data(0x04, 0); // master has a slave at IRQ 2 (binary)
  send_data(0x02, 1); // slave is at master's IRQ 2.

  // ICW4 (blah?)
  send_data(0x01, 0);
  send_data(0x01, 1);
}

static inline void send_command(uint8_t command, int pic) {
  outb(0x20 + (pic * 0x80), command);
}

static inline void send_data(uint8_t data, int pic) {
  outb(0x21 + (pic * 0x80), data);
}

static inline uint8_t recv_data(int pic) {
  return inb(0x21 + (pic * 0x80));
}
