#include <stdint.h>

#include "dt.h"
#include "interrupts.h"
#include "kprintf.h"
#include "multiboot.h"
#include "pagedir.h"
#include "pic.h"
#include "portio.h"
#include "screen.h"
#include "ssp.h"
#include "tss.h"

extern char kernel_stack[];

static struct tss kernel_tss;

static void init_paging(void);
static void init_kernel_tss(void);

extern volatile int got_atapi_irq;

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01
#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

/* void ide_write(unsigned char channel, unsigned char reg, unsigned char data) { */
/*   if (reg > 0x07 && reg < 0x0C) */
/*     ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN); */
/*   if (reg < 0x08) */
/*     outb(channels[channel].base  + reg - 0x00, data); */
/*   else if (reg < 0x0C) */
/*     outb(channels[channel].base  + reg - 0x06, data); */
/*   else if (reg < 0x0E) */
/*     outb(channels[channel].ctrl  + reg - 0x0A, data); */
/*   else if (reg < 0x16) */
/*     outb(channels[channel].bmide + reg - 0x0E, data); */
/*   if (reg > 0x07 && reg < 0x0C) */
/*     ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN); */
/* } */

unsigned char ide_polling() {
  // (I) Delay 400 nanosecond for BSY to be set:
  // -------------------------------------------------
  for(int i = 0; i < 4; i++) {
    inb(0x3f6);
  }

  // (II) Wait for BSY to be cleared:
  // -------------------------------------------------
  while (inb(0x1f7) & ATA_SR_BSY)
    ; // Wait for BSY to be zero.

  {
    unsigned char state = inb(0x1f7); // Read Status Register.

    // (III) Check For Errors:
    // -------------------------------------------------
    if (state & ATA_SR_ERR)
      return 2; // Error.

    // (IV) Check If Device fault:
    // -------------------------------------------------
    if (state & ATA_SR_DF)
      return 1; // Device Fault.

    // (V) Check DRQ:
    // -------------------------------------------------
    // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
    if ((state & ATA_SR_DRQ) == 0)
      return 3; // DRQ should be set
  }

  return 0; // No Error.
}


// kmain is the main entry point to the kernel after boot.S executes.
void kmain(int magic, multiboot_info_t* mbi) {
  uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  const uint32_t lba = 16;
  const uint8_t numsects = 1;
  const uint8_t slavebit = 0;
  const uint32_t words = 1024;
  unsigned char err;
  uint16_t buffer[1024] = {0};

  init_ssp();
  init_paging();
  init_kernel_tss();
  clearscreen();
  init_interrupts();
  init_pic(0x20, 0x28);

  __asm__ __volatile__ ("sti");

  // Enable IDE interrupts.
  got_atapi_irq = 0;
  outb(0x3f6, 0);

  // Prepare the command.
  atapi_packet[ 0] = 0xA8;
  atapi_packet[ 1] = 0x0;
  atapi_packet[ 2] = (lba >> 24) & 0xFF;
  atapi_packet[ 3] = (lba >> 16) & 0xFF;
  atapi_packet[ 4] = (lba >> 8) & 0xFF;
  atapi_packet[ 5] = (lba >> 0) & 0xFF;
  atapi_packet[ 6] = 0x0;
  atapi_packet[ 7] = 0x0;
  atapi_packet[ 8] = 0x0;
  atapi_packet[ 9] = numsects;
  atapi_packet[10] = 0x0;
  atapi_packet[11] = 0x0;

  // Select the drive.
  outb(0x1f6, slavebit << 4);

  // Delay.
  for(int i = 0; i < 4; i++) {
    inb(0x3f6);
  }

  // PIO mode.
  outb(0x1f1, 0);

  // Buffer size.
  outb(0x1f4, (words * 2) & 0xFF);
  outb(0x1f5, (words * 2) >> 8);

  // Send the packet command.
  outb(0x1f7, 0xA0);

  // Check for an error.
  if ((err = ide_polling())) {
    kprintf("error %d\n", err);
    __asm__ __volatile__ ("cli; hlt");
  }

  // Send the packet data.
  __asm__ __volatile__ ("rep   outsw" : : "c"(6), "d"(0x1f0), "S"(atapi_packet));   // Send Packet Data

  // Read the data.
  while (!got_atapi_irq)
    ;
  got_atapi_irq = 0;
  if ((err = ide_polling())) {
    kprintf("error %d\n", err);
    __asm__ __volatile__ ("cli; hlt");
  }
  insw(0x1f0, buffer, words);

  // (X): Waiting for an IRQ:
  // ------------------------------------------------------------------
  while (!got_atapi_irq)
    ;
  got_atapi_irq = 0;

  // (XI): Waiting for BSY & DRQ to clear:
  // ------------------------------------------------------------------
  while (inb(0x1f7) & 0x88)
    ;

  for (uint32_t i = 0; i < words; i++) {
    unsigned char a = buffer[i] & 0xFF;
    unsigned char b = (buffer[i] >> 8) & 0xFF;
    kprintf("%03lx %04x %c %c\n", i, buffer[i], (a >= 0x20 && a < 0x7f) ? a : '.', (b >= 0x20 && b < 0x7f) ? b : '.');
  }

  puts("OK!\n");

  while (1) {
    __asm__ __volatile__ ("pause");
  }


  puts("Done!\n");
  __asm__ __volatile__ ("cli; hlt");
}

static struct pte pagetable[1024] __attribute__ ((aligned(0x1000)));

static void init_paging(void) {
  // Use PTEs for the first page.
  kernel_pagedir[0].e.table.present = 1;
  kernel_pagedir[0].e.table.rw = 1;
  kernel_pagedir[0].e.table.us = 0;
  kernel_pagedir[0].e.table.pwt = 0;
  kernel_pagedir[0].e.table.pcd = 0;
  kernel_pagedir[0].e.table.accessed = 0;
  kernel_pagedir[0].e.table.reserved1 = 0;
  kernel_pagedir[0].e.table.ps = 0;
  kernel_pagedir[0].e.table.reserved2 = 0;
  kernel_pagedir[0].e.table.addr = (((unsigned int)pagetable) - 0xC0000000) >> 12;

  // Identity map the first megabyte for easy access to the BIOS, VGA, etc.
  for (int i = 0; i < 256; i++) {
    pagetable[i].present = 1;
    pagetable[i].rw = 1;
    pagetable[i].us = 0;
    pagetable[i].pwt = 0;
    pagetable[i].pcd = 0;
    pagetable[i].accessed = 0;
    pagetable[i].dirty = 0;
    pagetable[i].pat = 0;
    pagetable[i].global = 0;
    pagetable[i].reserved1 = 0;
    pagetable[i].addr = i;
  }

  // Unmap the second page.
  __builtin_memset(kernel_pagedir + 1, 0, sizeof(kernel_pagedir[1]));

  __asm__ __volatile__ ("mov %%cr3, %%eax; mov %%eax, %%cr3" : : : "eax");
}

static void init_kernel_tss(void) {
  gdt[5].limit_lo    = (((unsigned int)(sizeof(kernel_tss))) & 0x0000FFFF);
  gdt[5].base_lo     = (((unsigned int)(&kernel_tss)) & 0x00FFFFFF);
  gdt[5].type        = 9;
  gdt[5].reserved1   = 0;
  gdt[5].dpl         = 0;
  gdt[5].present     = 1;
  gdt[5].limit_hi    = (((unsigned int)(sizeof(kernel_tss))) & 0xFFFF0000) >> 16;
  gdt[5].available   = 0;
  gdt[5].reserved2   = 0;
  gdt[5].granularity = 0;
  gdt[5].base_hi     = (((unsigned int)(&kernel_tss)) & 0xFF000000) >> 24;

  kernel_tss.esp0   = (unsigned int)kernel_stack;
  kernel_tss.ss0    = 0x10;
  __asm__ __volatile__ ("movl %%cr3, %0" : "=r" (kernel_tss.cr3));
  kernel_tss.eip    = (unsigned int)kmain;
  kernel_tss.eflags = 0x0002;
  kernel_tss.esp    = (unsigned int)kernel_stack;
  kernel_tss.cs     = 0x08;
  kernel_tss.ss     = 0x10;
  kernel_tss.ds     = 0x10;
  kernel_tss.es     = 0x10;
  kernel_tss.fs     = 0x10;
  kernel_tss.gs     = 0x10;

  __asm__ __volatile__ ("mov $0x28, %%ax; ltr %%ax" : : : "ax");
}
