#include "elf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "log.h"
#include "mmu/address_space.h"
#include "mmu/heap.h"
#include "mmu/mmu.h"
#include "process.h"
#include "string.h"

#define LOG(...) log("ELF", __VA_ARGS__)
#define PANIC(...) panic("ELF", __VA_ARGS__)

static bool check_elf_header(const struct Elf32_Ehdr* ehdr);
static bool map_segments(const struct Elf32_Ehdr* ehdr, uintptr_t* end);
static int make_envp_argv(char* out[], char* const in[]);

void elf_exec(struct Elf32_Ehdr* ehdr, char* const argv[], char* const envp[]) {
  uintptr_t end = 0;
  char** kernel_argv = NULL;
  int argc;
  char** kernel_envp = NULL;
  int envc;

  // Copy the argv and envp arrays into the kernel so that the user's
  // address space can be safely wiped.
  argc = 0;
  while (argv[argc] != NULL) {
    argc++;
  }
  kernel_argv = kmalloc(sizeof(char*) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    kernel_argv[i] = strdup(argv[i]);
  }
  kernel_argv[argc] = NULL;

  envc = 0;
  while (envp[envc] != NULL) {
    envc++;
  }
  kernel_envp = kmalloc(sizeof(char*) * (envc + 1));
  for (int i = 0; i < envc; i++) {
    kernel_envp[i] = strdup(envp[i]);
  }
  kernel_envp[envc] = NULL;

  // Reset the address space to prepare for the new process.
  mmu_free_address_space();

  // Parse the elf header.
  if (!check_elf_header(ehdr)) {
    PANIC("Bad elf header\n");
  }

  LOG("eip  is at %08lx\n", ehdr->e_entry);
  current_process->tss.eip = ehdr->e_entry;

  // Map the segments into the address space.
  if (!map_segments(ehdr, &end)) {
    PANIC("Unable to map segments\n");
  }
  kfree(ehdr);

  // Copy envp into the address space.
  mmu_map_page((void*)end, kPagePresent | kPageUser | kPageWrite);
  LOG("envp is at %08lx\n", end);
  current_process->envp = (char**)end;
  (void)make_envp_argv(current_process->envp, kernel_envp);
  kfree(kernel_envp);
  end += PAGESIZE;

  // Copy argv into the address space.
  mmu_map_page((void*)end, kPagePresent | kPageUser | kPageWrite);
  LOG("argv is at %08lx\n", end);
  current_process->argv = (char**)end;
  current_process->argc = make_envp_argv(current_process->argv, kernel_argv);
  kfree(kernel_argv);
  end += PAGESIZE;

  // Create the stack for the new process.
  mmu_map_page((void*)end, kPagePresent | kPageUser | kPageWrite);
  LOG("esp  is at %08lx\n", end);
  {
    uint32_t* new_stack = (uint32_t*)(end + PAGESIZE);
    *(--new_stack) = (uint32_t)current_process->envp;
    *(--new_stack) = (uint32_t)current_process->argv;
    *(--new_stack) = (uint32_t)current_process->argc;
    current_process->tss.esp = (uintptr_t)new_stack;
  }
  end += PAGESIZE;

  // Set the program break;
  current_process->program_break = end;
  LOG("Program break is at %08lx\n", end);
  end += PAGESIZE;

  // Switch to the new process in ring 3.
  process_start();
}

static bool check_elf_header(const struct Elf32_Ehdr* ehdr) {
  if (ehdr->e_ident[0] != EI_MAG0 ||
      ehdr->e_ident[1] != EI_MAG1 ||
      ehdr->e_ident[2] != EI_MAG2 ||
      ehdr->e_ident[3] != EI_MAG3) {
    LOG("Bad magic\n");
    return false;
  }
  if (ehdr->e_ident[4] != ELFCLASS32) {
    LOG("Not ELFCLASS32\n");
    return false;
  }
  if (ehdr->e_ident[5] != ELFDATA2LSB) {
    LOG("Not ELFDATA2LSB\n");
    return false;
  }
  if (ehdr->e_ident[6] != EV_CURRENT) {
    LOG("Not EV_CURRENT\n");
    return false;
  }
  if (ehdr->e_type != ET_EXEC) {
    LOG("Not ET_EXEC\n");
    return false;
  }
  if (ehdr->e_machine != EM_386) {
    LOG("Not EM_386\n");
    return false;
  }
  if (ehdr->e_version != EV_CURRENT) {
    LOG("Not EV_CURRENT\n");
    return false;
  }
  if (ehdr->e_phoff == 0) {
    LOG("No program header table\n");
    return false;
  }
  return true;
}

static bool map_segments(const struct Elf32_Ehdr* ehdr, uintptr_t* end) {
  const struct Elf32_Phdr* phdr = (struct Elf32_Phdr*)((uintptr_t)ehdr + ehdr->e_phoff);
  for (int i = 0; i < ehdr->e_phnum; i++) {
    switch (phdr->p_type) {
    case PT_NULL:
      break;
    case PT_LOAD: {
      // TODO(zhirsch): Figure out phdr->p_align.
      const uintptr_t src_base = ((uintptr_t)ehdr + phdr->p_offset);
      const uintptr_t dst_base = phdr->p_vaddr;
      const uintptr_t bss_base = phdr->p_vaddr + phdr->p_filesz;
      const size_t    bss_size = phdr->p_memsz - phdr->p_filesz;
      const enum PageFlags perm = (phdr->p_flags == 0x5) ? 0 : kPageWrite;
      uintptr_t dst;
      // Map the pages for the segment.
      for (dst = dst_base; dst < phdr->p_vaddr + phdr->p_memsz; dst += PAGESIZE) {
        mmu_map_page((void*)dst, kPagePresent | kPageUser | kPageWrite);
      }
      // Copy the data into the segment.
      memcpy((void*)dst_base, (void*)src_base, phdr->p_filesz);
      // Zero out the BSS.
      memset((void*)bss_base, 0, bss_size);
      // Set the access permissions for the segment.
      for (dst = dst_base; dst < phdr->p_vaddr + phdr->p_memsz; dst += PAGESIZE) {
        mmu_set_pte_flags((void*)dst, kPagePresent | kPageUser | perm);
      }
      *end = dst;
      break;
    }
    default:
      LOG("Unhandled program header type %lx\n", phdr->p_type);
      return false;
    }
    phdr++;
  }
  *end = (uintptr_t)mmu_round_to_next_page((void*)*end);
  return true;
}

static int make_envp_argv(char* out[], char* const in[]) {
  int i;
  char* ptr = (char*)out;

  // Count the number of entries to copy.
  for (i = 0; in[i] != NULL; i++) {
    ptr += sizeof(*out);
  }
  ptr += sizeof(*out);

  // Write the pointers to the strings.
  for (i = 0; in[i] != NULL; i++) {
    out[i] = strcpy(ptr, in[i]);
    ptr += strlen(in[i]) + 2;
  }
  out[i] = NULL;
  return i;
}
