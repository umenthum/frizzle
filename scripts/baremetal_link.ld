OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
    . = 0x80001000;
    .text : {
        *(.text._start);
        *(.text);
    }
    .data : { *(.data) }
    _end = .;
}

