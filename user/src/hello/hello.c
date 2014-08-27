#include <stdio.h>

int main(int argc, char* argv[]) {
  int ret;
  ret = fputs("Hello, world!\n", stderr);
  if (ret == -1) {
    return 17;
  }
  return 0;
}
