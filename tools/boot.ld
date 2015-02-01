/*
 * boot.ld 
 */
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x7C00;
    .startup : {
        *bootasm.o(.text)
    }

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }
    /DISCARD/ : { *(.comment) *(.eh_frame)  }
}
