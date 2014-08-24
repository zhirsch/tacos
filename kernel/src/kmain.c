#include <stdint.h>
#include <string.h>

#include "dt.h"
#include "ide/ide.h"
#include "interrupts.h"
#include "kmalloc.h"
#include "kprintf.h"
#include "mmu.h"
#include "multiboot.h"
#include "panic.h"
#include "pic.h"
#include "portio.h"
#include "screen.h"
#include "snprintf.h"
#include "ssp.h"
#include "tss.h"

static void init_kernel_tss(void);

void iso9660_print_paths(const uint8_t* dirrec, const char* parent) {
  char* path;
  uint8_t* buffer;
  uint32_t extent_pos, extent_len;
  int i;

  if (__builtin_strcmp(parent, "/") == 0) {
    const int size = 1 + dirrec[32] + 1;
    path = kmalloc(size);
    snprintf(path, size, "/%.*s", dirrec[32], dirrec+33);
  } else {
    const int size = strlen(parent) + 1 + dirrec[32] + 1;
    path = kmalloc(size);
    snprintf(path, size, "%s/%.*s", parent, dirrec[32], dirrec+33);
  }
  __builtin_memcpy(&extent_pos, dirrec + 2, 4);
  __builtin_memcpy(&extent_len, dirrec + 10, 4);
  kprintf("%02x %-40s (%08lx, %08lx)\n", dirrec[25], path, extent_pos, extent_len);
  kfree(path);

  if (!(dirrec[25] & 0x2)) {
    return;
  }

  buffer = kmalloc(extent_len);
  if (!ide_read(0, 0, buffer, extent_pos, extent_len / 2)) {
    kprintf("Error!\n");
    kfree(buffer);
    return;
  }

  // First record is for ".", second record is for ".."; skip them.
  i = 0;
  i += buffer[i];
  i += buffer[i];
  while (buffer[i] != 0) {
    if (__builtin_strcmp(parent, "/") == 0) {
      const int size = 1 + dirrec[32] + 1;
      path = kmalloc(size);
      snprintf(path, size, "/%.*s", dirrec[32], dirrec+33);
    } else {
      const int size = strlen(parent) + 1 + dirrec[32] + 1;
      path = kmalloc(size);
      snprintf(path, size, "%s/%.*s", parent, dirrec[32], dirrec+33);
    }
    iso9660_print_paths(buffer+i, path);
    kfree(path);
    i += buffer[i];
  }
  kfree(buffer);
}

// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  init_ssp();

  // Clear the screen.
  clearscreen();

  // Initialize the address space.
  init_mmu(mbi);
  init_kernel_tss();

  // Enable interrupts.
  init_interrupts();
  init_pic(0x20, 0x28);
  __asm__ __volatile__ ("sti");

  // Initialize IDE.
  init_ide();

  {
    void* ptr = kmalloc(10 << 20);
    kfree(ptr);
  }

  // Read the first non-reserved sector of the CD-ROM.
  {
    int lba = 0x10;
    uint8_t* buffer = kmalloc(2048);
    while (1) {
      if (!ide_read(0, 0, buffer, lba, 1024)) {
        kprintf("Uh-oh @%x\n", lba);
        break;
      } else if (buffer[0] == 0xff) {
        break;
      }
      kprintf("Type           = %02x\n", buffer[0]);
      kprintf("ID             = %.*s\n", 5, buffer+1);
      kprintf("Version        = %02x\n", buffer[6]);
      if (buffer[0] == 0x01) {
        iso9660_print_paths(buffer+156, "/");
      }
      lba++;
    }
    kfree(buffer);
  }

  panic0("Done!\n");
}

static void init_kernel_tss(void) {
  static struct tss kernel_tss __attribute__ ((aligned(0x1000)));

  gdt[5].limit_lo    = (((uintptr_t)(sizeof(kernel_tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((uintptr_t)(&kernel_tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((uintptr_t)(sizeof(kernel_tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((uintptr_t)(&kernel_tss)) & 0xFF000000) >> 24;

  // The kernel's TSS is set to zeros because:
  //   1) There should never be a switch to this task from a different
  //      privilege level.
  // and
  //   2) The necessary fields will be filled in upon the first switch to a
  //      different task.
  // The exception to this is cr3, because the processor doesn't save it on a
  // task switch.
  memset(&kernel_tss, 0, sizeof(kernel_tss));
  __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (kernel_tss.cr3));

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
