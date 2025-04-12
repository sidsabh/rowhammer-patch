SECTIONS {
  . = 0x400000;

  .text : {
    __text_start = .;
    *(.text)
    *(.text.*)
    __text_end = .;
  }
  .verify : {
    *(.verify)
  }

  .init_array : {
    KEEP(*(.init_array))  /* libc will call these */
  }

  .rodata : { *(.rodata*) }
  .data : { *(.data*) }
  .bss : { *(.bss*) *(COMMON) }

  /DISCARD/ : { *(.comment) *(.note*) }
}
