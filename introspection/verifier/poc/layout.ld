SECTIONS {
  . = 0x400000;

  .text : {
    __text_start = .;
    *(.text)
    *(.text.*)
    __text_end = .;
    PROVIDE(__global_pointer$ = __text_end + 0x1000);
  }

. = 0x800000;

  .verify : {
    *(.verify)
  }

  .init_array : { KEEP(*(.init_array)) }
  .rodata    : { *(.rodata*) }
  .data      : { *(.data*) }
  .bss       : { *(.bss*) *(COMMON) }

  /DISCARD/ : { *(.comment) *(.note*) }
}
