#include "interrupts.h"
#include "log.h"
#include "process.h"
#include "syscall.h"

#define LOG(...) log("SYSCALL [GETPGID]", __VA_ARGS__)

void syscall_getpgid(struct isr_frame* frame) {
  syscall_in0();
  syscall_out(frame, current_process->pgid, "%ld");
}
