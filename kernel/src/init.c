#include "init.h"

#include <stddef.h>

#include "bits/fcntl.h"
#include "bits/stat.h"
#include "elf.h"
#include "iso9660.h"
#include "log.h"
#include "mmu/heap.h"
#include "process.h"
#include "string.h"
#include "tty.h"

#define LOG(...) log("INIT", __VA_ARGS__)
#define PANIC(...) panic("INIT", __VA_ARGS__)

static char* get_init_path(const char* cmdline) {
  char* initpath = NULL;
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

void start_init(const char* cmdline) {
  char* initpath = NULL;
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

  current_process->tss.cs = SEGMENT_USER_CODE;
  current_process->tss.ss = SEGMENT_USER_DATA;
  current_process->tss.ds = current_process->tss.ss;
  current_process->tss.es = current_process->tss.ss;
  current_process->tss.fs = current_process->tss.ss;
  current_process->tss.gs = current_process->tss.ss;
  current_process->tss.eflags = 0x0200;  // IF=1

  current_process->tty = tty_get(0);
  tty_setpgid(current_process->tty, current_process->pgid);

  // stdin
  current_process->fds[FD_STDIN].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDIN].u.tty = current_process->tty;
  current_process->fds[FD_STDIN].mode = O_RDONLY;
  
  // stdout
  current_process->fds[FD_STDOUT].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDOUT].u.tty = current_process->tty;
  current_process->fds[FD_STDOUT].mode = O_WRONLY;
    
  // stderr
  current_process->fds[FD_STDERR].type = PROCESS_FD_TTY;
  current_process->fds[FD_STDERR].u.tty = current_process->tty;
  current_process->fds[FD_STDERR].mode = O_WRONLY;

  // Execute the init program.
  {
    char* const argv[] = {
      initpath,
      "-i",
      NULL,
    };
    char* const envp[] = {
      "OS=TacOS",
      "AUTHOR=zhirsch",
      NULL,
    };
    elf_exec(elf, argv, envp);
  }
}
