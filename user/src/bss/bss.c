#include <stdint.h>
#include <stdio.h>

static char bss[0x4000];

int main(int argc, char* argv[]) {
  for (size_t i = 0; i < sizeof(bss); i++) {
    bss[i] = 'a';
  }
  bss[0x4000 - 1] = '\0';
  return puts(bss);
}
