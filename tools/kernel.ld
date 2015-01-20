/*
 *  kernel.ld is a script for xos
 *
 * 首先声明内核程序的入口地址符号是"kernel_init"
 * 该脚本告诉GNU ld 如何构造内核镜像
 */

ENTRY(kernel_init)
SECTIONS
{
    /* 定位器符号置为0x100000 */
    . = 0x100000;

    /* 将所有输入文件的.text , .stub等段合并为一个.text段,该段的起始地址有定位器决定, 即为0x100000 */
    .text :
    {
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }

    /* PROVIDE(etext = .) 表示程序中引用了etext,却没有定义它,那么链接器就是用链接器脚本中的定义 */
    PROVIDE(etext = .);

    /* 采用4k页对齐*/
    . = ALIGN(0x1000);

    .data : {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }

    /DISCARD/ : { *(.comment) *(.eh_frame)  }
}
