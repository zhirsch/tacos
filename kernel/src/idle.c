#include "idle.h"

#include "log.h"
#include "mmu/heap.h"
#include "process.h"
#include "scheduler.h"

#define LOG(...) log("ELF", __VA_ARGS__)

void start_idle(void) {
	do {
		scheduler_yield();
		__asm__ __volatile__ ("sti; hlt; cli");
	} while (1);
}
