OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;
    .text : {
        *(.text._entry);
        *(.text);
    }
    .data : { *(.data) }
    _end = .;
}

