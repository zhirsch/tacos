#include <stdio.h>

int main(int argc, char* argv[]) {
  int ret;
  ret = puts("Hello, world!\n");
  if (ret == -1) {
    return 17;
  }
  return 0;
}
