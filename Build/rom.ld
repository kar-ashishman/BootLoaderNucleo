MEMORY {
    FLASH(xr) : ORIGIN = 0x00000000, LENGTH = 50K
    SRAM(xrw) : ORIGIN = 0x20000000, LENGTH = 8K
}

SECTIONS {
    .text : {
        startup.o (.vector_table)
        *(.text)
        . = ALIGN(4);
        _etext = .;
    }>FLASH
}