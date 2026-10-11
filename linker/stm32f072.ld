/* === linker.ld ===
 * Memory map for STM32F072 (bare-metal)
 * FLASH : 0x0800_0000, 128 KB
 * RAM   : 0x2000_00C0, 16 KB − 0xC0
 */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x20000
  RAM   (rwx): ORIGIN = 0x200000C0, LENGTH = 0x3F40
}

/* Stack top (used by startup vector) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Symbols used by startup code to copy/clear data */
_sidata = LOADADDR(.data);
_sdata  = ADDR(.data);
_edata  = _sdata + SIZEOF(.data);
_sbss   = ADDR(.bss);
_ebss   = _sbss + SIZEOF(.bss);

SECTIONS
{
  /* Interrupt vector table (first thing in Flash) */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  /* Code + constants */
  .text :
  {
    *(.text*)         /* all code */
    *(.rodata*)       /* read-only const data */
    KEEP(*(.init))
    KEEP(*(.fini))
  } > FLASH

  /* Initialized data — load in Flash, run in RAM */
  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  /* Zero-initialized data (BSS) */
  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  /* Simple heap/stack layout (top of RAM is stack) */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(_heap_start = .);
    . = . + 0x400;      /* small heap (1 KB) */
    PROVIDE(_heap_end = .);
  } > RAM
}
