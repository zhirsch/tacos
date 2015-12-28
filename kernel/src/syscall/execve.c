#include "syscalls/syscalls.h"

#include "bits/errno.h"
#include "bits/fcntl.h"

#include "elf.h"
#include "interrupts.h"
#include "iso9660.h"
#include "log.h"
#include "mmu/heap.h"

#define LOG(...) log("SYSCALL [execve]", __VA_ARGS__)
#define PANIC(...) panic("SYSCALL [execve]", __VA_ARGS__)

int sys_execve(const char* filename, char* const argv[], char* const envp[],
               struct isr_frame* frame) {
  struct file* file = NULL;
  int c;
  struct stat st;
  struct Elf32_Ehdr* elf = NULL;

  // Open the file.
  c = iso9660_open(filename, O_RDONLY, &file);
  if (c != 0) {
    return c;
  }

  // Stat the binary.
  c = iso9660_fstat(file, &st);
  if (c != 0) {
    return c;
  }

  // Allocate memory for the binary.
  elf = kmalloc(st.st_size);
  if (elf == NULL) {
    LOG("Unable to allocate memory to read %s\n", filename);
    return -ENOMEM;
  }

  // Read the binary.
  c = iso9660_pread(file, elf, st.st_size, 0);
  if (c < 0) {
    LOG("Failed to read %s: %d\n", filename, c);
    return c;
  }
  if (c != st.st_size) {
    PANIC("Unable to read all of %s: %d of %ld\n", filename, c, (unsigned long)st.st_size);
  }

  // Close the file.
  c = iso9660_close(file);
  if (c != 0) {
    LOG("Unable to close %s: %d\n", filename, c);
    return c;
  }

  LOG("%s loaded at %08lx\n", filename, (uintptr_t)elf);
  elf_exec(elf, argv, envp);
}
