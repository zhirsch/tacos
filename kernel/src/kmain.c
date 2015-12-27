#include <stdint.h>

#include "bits/fcntl.h"

#include "dt.h"
#include "elf.h"
#include "file.h"
#include "fpu.h"
#include "ide/ide.h"
#include "interrupts.h"
#include "iso9660.h"
#include "ldsyms.h"
#include "log.h"
#include "mmu/common.h"
#include "mmu/heap.h"
#include "mmu/linear.h"
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
static void start_init(const char* cmdline) __attribute__ ((noreturn));

static char cmdline[256];

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
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

  // Initialize IDE.
  init_ide();

  // Start the init program.  This doesn't return.
  start_init(cmdline);
}

static void announce(void) {
  for (int i = 0; i < NUM_TTYS; i++) {
    char ann[300];
    const size_t sz = snprintf(ann, sizeof(ann), "TacOS! (TTY %d)\n", i);
    tty_write(tty_get(i), ann, sz);
  }
}

static const char* get_init_path(const char* cmdline) {
  const char* initpath = NULL;
  // Parse the command line to find the name of the init program.
  // TODO(zhirsch): Allow for init= to be anywhere in the command line, not just
  // at the end.
  initpath = strstr(cmdline, " init=");
  if (initpath == NULL) {
    return NULL;
  }
  initpath += strlen(" init=");
  return initpath;
}

static void start_init(const char* cmdline) {
  const char* initpath = NULL;
  struct file* file = NULL;
  int c;
  struct stat st;
  struct Elf32_Ehdr* elf = NULL;

  // Parse the kernel command line to get the init path.
  initpath = get_init_path(cmdline);
  if (initpath == NULL) {
    PANIC("Unable to find init= in command line\n");
  }
  LOG("Found init program \"%s\"\n", initpath);

  // Open the file.
  c = iso9660_open(initpath, O_RDONLY, &file);
  if (c != 0) {
    PANIC("Unable to open %s: %d\n", initpath, c);
  }

  // Stat the binary.
  c = iso9660_fstat(file, &st);
  if (c != 0) {
    PANIC("Unable to stat %s: %d\n", initpath, c);
  }

  // Allocate memory for the binary.
  elf = kmalloc(st.st_size);
  if (elf == NULL) {
    PANIC("Unable to allocate memory for the init program\n");
  }

  // Read the binary.
  c = iso9660_pread(file, elf, st.st_size, 0);
  if (c < 0) {
    PANIC("Failed to read %s: %d\n", initpath, c);
  }
  if (c != st.st_size) {
    PANIC("Unable to read all of %s: %d of %ld\n", initpath, c, (unsigned long)st.st_size);
  }

  // Close the file.
  c = iso9660_close(file);
  if (c != 0) {
    PANIC("Unable to close %s: %d\n", initpath, c);
  }

  LOG("%s loaded at %08lx\n", initpath, (uintptr_t)elf);

  // Set the current process.
  current_process = kcalloc(1, sizeof(*current_process));
  current_process->pid        = process_next_pid();
  current_process->ppid       = 0;
  current_process->pgid       = process_next_pgid();
  current_process->uid        = 0;  // root
  current_process->euid       = 0;  // root
  current_process->gid        = 0;  // root
  current_process->egid       = 0;  // root
  current_process->state      = PROCESS_ALIVE;
  current_process->wait_state = PROCESS_WAIT_NONE;
  current_process->status     = 0;
  current_process->tty        = tty_get(0);
  current_process->cwd        = "/";
  current_process->tss.cs     = SEGMENT_USER_CODE;
  current_process->tss.ss     = SEGMENT_USER_DATA;
  current_process->tss.ds     = SEGMENT_USER_DATA;
  current_process->tss.es     = SEGMENT_USER_DATA;
  current_process->tss.fs     = SEGMENT_USER_DATA;
  current_process->tss.gs     = SEGMENT_USER_DATA;
  current_process->tss.cr3    = (uintptr_t)lmmu_get_cr3();
  current_process->tss.eflags = 0x0200;  // IF=1

  // Since this is the first process, link it to itself.
  current_process->next = current_process;

  // stdin
  current_process->fds[FD_STDIN].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDIN].u.tty = tty_get(0);
  current_process->fds[FD_STDIN].mode = O_RDONLY;
  // stdout
  current_process->fds[FD_STDOUT].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDOUT].u.tty = tty_get(0);
  current_process->fds[FD_STDOUT].mode = O_WRONLY;
  // stderr
  current_process->fds[FD_STDERR].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDERR].u.tty = tty_get(0);
  current_process->fds[FD_STDERR].mode = O_WRONLY;

  tty_setpgid(current_process->tty, current_process->pgid);

  // Execute the init program.
  {
    const char* argv[] = {
      initpath,
      "-i",
      NULL,
    };
    const char* envp[] = {
      "OS=TacOS",
      NULL,
    };
    elf_exec(elf, argv, envp);
  }
}

extern const void kernel_stack_start;

static void init_tss(void) {
  static struct tss tss __attribute__ ((aligned(PAGESIZE)));

  gdt[5].limit_lo    = (((uintptr_t)(sizeof(tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((uintptr_t)(&tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((uintptr_t)(sizeof(tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((uintptr_t)(&tss)) & 0xFF000000) >> 24;

  memset(&tss, 0, sizeof(tss));
  tss.ss0  = SEGMENT_KERNEL_DATA;
  tss.esp0 = (uintptr_t)&kernel_stack_start;

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
