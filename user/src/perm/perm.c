int main() {
  char* p = (char*)0x400000;
  while (1) {
    *(p++) = 0;
  }
  return 0;
}
