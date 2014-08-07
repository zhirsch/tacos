#include "dt.h"

#include <stdint.h>

struct gdt* get_gdt_entry(uint16_t selector) {
  const uint16_t pos = selector >> 3;
  return gdt + pos;
}
