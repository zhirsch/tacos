#include "elf.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kmalloc.h"
#include "log.h"
#include "mmu.h"
#include "process.h"
#include "string.h"

#define LOG(...) log("ELF", __VA_ARGS__)
#define PANIC(...) panic("ELF", __VA_ARGS__)

static bool check_elf_header(const struct Elf32_Ehdr* ehdr);
static bool map_segments(const struct Elf32_Ehdr* ehdr, uintptr_t* end);

void elf_exec(struct Elf32_Ehdr* ehdr, const char* argv[], const char* envp[]) {
  uintptr_t end = 0;

  // Reset the address space to prepare for the new process.
  mmu_reset_cr3();

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
  mmu_map_page(mmu_acquire_physical_page(), (void*)end, 0x1 | 0x2 | 0x4);
  LOG("envp is at %08lx\n", end);
  current_process->envp = (char**)end;
  {
    int i;
    char* envs = (char*)end;
    current_process->envp = (char**)envs;
    for (i = 0; envp[i] != NULL; i++) {
      envs += sizeof(char**);
    }
    envs += sizeof(char**);
    for (i = 0; envp[i] != NULL; i++) {
      strcpy(envs, envp[i]);
      current_process->envp[i] = envs;
      envs += strlen(envp[i]) + 1;
    }
    current_process->envp[i] = NULL;
  }
  end += PAGESIZE;

  // Copy argv into the address space.
  mmu_map_page(mmu_acquire_physical_page(), (void*)end, 0x1 | 0x2 | 0x4);
  LOG("argv is at %08lx\n", end);
  current_process->argv = (char**)end;
  {
    int i;
    char* args = (char*)end;
    current_process->argv = (char**)args;
    for (i = 0; argv[i] != NULL; i++) {
      args += sizeof(char**);
    }
    args += sizeof(char**);
    for (i = 0; argv[i] != NULL; i++) {
      strcpy(args, argv[i]);
      current_process->argv[i] = args;
      args += strlen(argv[i]) + 1;
    }
    current_process->argv[i] = NULL;
    current_process->argc = i;
  }
  end += PAGESIZE;

  // Create the stack for the new process.
  mmu_map_page(mmu_acquire_physical_page(), (void*)end, 0x1 | 0x2 | 0x4);
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
  {
    extern void switch_to_ring3(uint16_t cs, uint32_t eip, uint16_t ss, uint32_t esp, uint32_t eflags) __attribute__ ((noreturn));
    switch_to_ring3(current_process->tss.cs,
                    current_process->tss.eip,
                    current_process->tss.ss,
                    current_process->tss.esp,
                    current_process->tss.eflags);
  }
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
      const uint8_t   perm     = (phdr->p_flags == 0x5) ? 0x1 : 0x3;
      uintptr_t dst;
      // Map the pages for the segment.
      for (dst = dst_base; dst < phdr->p_vaddr + phdr->p_memsz; dst += PAGESIZE) {
        const uintptr_t paddr = (uintptr_t)mmu_acquire_physical_page();
        mmu_map_page((void*)paddr, (void*)dst, 0x1 | 0x2 | 0x4);
      }
      // Copy the data into the segment.
      memcpy((void*)dst_base, (void*)src_base, phdr->p_filesz);
      // Zero out the BSS.
      memset((void*)bss_base, 0, bss_size);
      // Set the access permissions for the segment.
      for (dst = dst_base ;dst < phdr->p_vaddr + phdr->p_memsz; dst += PAGESIZE) {
        mmu_set_page_flags((void*)dst, perm | 0x4);
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
  *end = mmu_round_to_next_page(*end);
  return true;
}
