OUTPUT_FORMAT("elf32-i386")
ENTRY("_start")
STARTUP(bld/arch/i386/boot/boot.o);

SECTIONS
{
  . = 0xC0100000;

  kernel_start = ALIGN(0x1000);

  .text ALIGN(0x1000) : AT(ADDR(.text) - 0xC0000000)
  {
    *(.text)
  }

  .data ALIGN(0x1000) : AT(ADDR(.data) - 0xC0000000)
  {
    *(.data)
    *(.rodata*)
  }

  .bss  ALIGN(0x1000) : AT(ADDR(.bss ) - 0xC0000000)
  {
    *(COMMON*)
    *(.bss)
  }

  kernel_end = ALIGN(0x1000);
}