#include "syscall/_exit.h"

#include "panic.h"
#include "tss.h"

void syscall__exit(struct tss* prev_tss) {
  panic("Process terminated with status %d\n", prev_tss->ebx);
}
