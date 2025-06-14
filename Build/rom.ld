/* Entry point for Debugger */
ENTRY(Reset_Handler)

/* name(attr) : ORIGIN/org = 0x... , LENGTH/len = 0x....; */
/* attributes are R readonly, W readwrite, X executable, A allocated */

MEMORY {
    FLASH(rx) : org = 0x08000000, len = 0x1FFFFF
    SRAM(rwx) : org = 0x20000000, len = 128K
}

/* >(VMA) AT>(LMA) - VMA where loader loads data to execute */
/* location counter . always tracks VMA not LMA */
/* ALIGN aligns address to boundary */
SECTIONS {
    .text : {
        *(.vector_table)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    }>FLASH

    .data : {
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    }>SRAM AT>FLASH

    .bss : {
        _sbss = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
}