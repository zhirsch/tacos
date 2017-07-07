#include <stdint.h>

#include "dt.h"
#include "drivers/keyboard.h"
#include "elf.h"
#include "file.h"
#include "fpu.h"
#include "ide/ide.h"
#include "idle.h"
#include "init.h"
#include "interrupts.h"
#include "iso9660.h"
#include "ldsyms.h"
#include "log.h"
#include "mmu/heap.h"
#include "mmu/mmu.h"
#include "multiboot.h"
#include "pic.h"
#include "portio.h"
#include "process.h"
#include "screen.h"
#include "snprintf.h"
#include "ssp.h"
#include "string.h"
#include "syscall.h"
#include "tss.h"
#include "tty.h"

#define LOG(...) log("KMAIN", __VA_ARGS__)
#define PANIC(...) panic("KMAIN", __VA_ARGS__)

static void init_tss(void);
static void announce(void);

static char cmdline[256];

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  struct process* init = NULL;
  struct process* idle = NULL;

  init_ssp();

  // Copy the command line into a buffer with a valid virtual address.  The boot
  // loader stores a physical address in mbi->cmdline, which won't be accessible
  // after init_mmu.
  strncpy(cmdline, (const char*)mbi->cmdline, sizeof(cmdline) - 1);
  cmdline[sizeof(cmdline)-1] = '\0';

  // Initialize the address space.
  init_mmu(mbi);
  init_tss();

  // Initialize the FPU.
  init_fpu();

  // Initialize the consoles.
  init_tty();
  screen_clear();

  // Announce the OS.
  announce();

  // Initialize interrupts.
  init_interrupts();
  init_pic(0x20, 0x28);
  init_syscalls();
  init_keyboard();

  // Initialize IDE.
  init_ide();

  // Create the processes.
  init = process_create(start_init, NULL, cmdline, NULL);
  idle = process_create(start_idle, NULL, NULL);

  // Link the processes together.
  init->next = idle;
  idle->next = init;

  // Start the init process.
  current_process = init;
  {
    struct tss dummy;
    process_switch(&dummy, &current_process->tss);
  }
}

static void announce(void) {
  for (int i = 0; i < NUM_TTYS; i++) {
    char ann[300];
    const size_t sz = snprintf(ann, sizeof(ann), "TacOS! (TTY %d)\n", i);
    tty_write(tty_get(i), ann, sz);
  }
}

static void init_tss(void) {
  gdt[5].limit_lo    = (((uintptr_t)(sizeof(g_tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((uintptr_t)(&g_tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((uintptr_t)(sizeof(g_tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((uintptr_t)(&g_tss)) & 0xFF000000) >> 24;

  memset(&g_tss, 0, sizeof(g_tss));
  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
