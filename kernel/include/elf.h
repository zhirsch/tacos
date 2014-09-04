#ifndef ELF_H
#define ELF_H

#include <stdint.h>

#define EI_NIDENT   16
#define EI_MAG0     0x7f
#define EI_MAG1     'E'
#define EI_MAG2     'L'
#define EI_MAG3     'F'

#define ELFCLASS32 1

#define ELFDATA2LSB 1

#define EV_CURRENT 1

#define ET_EXEC 2

#define EM_386 3

#define PT_NULL 0
#define PT_LOAD 1

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;

struct Elf32_Ehdr {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half    e_type;
  Elf32_Half    e_machine;
  Elf32_Word    e_version;
  Elf32_Addr    e_entry;
  Elf32_Off     e_phoff;
  Elf32_Off     e_shoff;
  Elf32_Word    e_flags;
  Elf32_Half    e_ehsize;
  Elf32_Half    e_phentsize;
  Elf32_Half    e_phnum;
  Elf32_Half    e_shentsize;
  Elf32_Half    e_shnum;
  Elf32_Half    e_shstrndx;
} Elf32_Ehdr;

struct Elf32_Phdr {
  Elf32_Word p_type;
  Elf32_Off  p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
};

// Replaces the current process with the one described by the ELF header.  Takes
// ownership of ehdr.
void elf_exec(struct Elf32_Ehdr* ehdr, const char* argv[], const char* envp[]) __attribute__ ((noreturn));

#endif /* ELF_H */
