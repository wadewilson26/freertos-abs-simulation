MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector)) /* 1. The Vector Table must go first */
        *(.text*)            /* 2. Your actual C code instructions */
        *(.rodata*)          /* 3. Read-only variables (const) */
        _etext = .;          /* Mark the end of the instructions */
    } > FLASH

    .data : AT(_etext)
    {
        _data = .;           /* Mark the start of RAM data */
        *(.data*)            /* 4. Initialized variables (e.g., int x = 5;) */
        _edata = .;          /* Mark the end of RAM data */
    } > RAM

    .bss :
    {
        _bss = .;            /* Mark the start of empty variables */
        *(.bss*)             /* 5. Uninitialized variables (e.g., int y;) */
        *(COMMON)
        _ebss = .;           /* Mark the end of empty variables */
    } > RAM

    /* NEW 7: Force the linker to permanently delete hidden C++ exception tables */
    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.extab*)
    }

    /* 8. Define the top of the Stack for the CPU */
    PROVIDE( _estack = ORIGIN(RAM) + LENGTH(RAM) );
}