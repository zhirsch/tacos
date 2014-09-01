#include <stdint.h>

#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "sbrk.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [SBRK]", __VA_ARGS__)

void syscall_sbrk(struct isr_frame* frame) {
  const uintptr_t vaddr = current_process->program_break;
  syscall_in1(frame, intptr_t, increment, "%08lx");

  if (increment < 0) {
    sbrk_shrink(&current_process->program_break, -increment);
  } else if (increment > 0) {
    sbrk_grow(&current_process->program_break, increment);
  }
  syscall_out(frame, vaddr, "%08lx");
}
