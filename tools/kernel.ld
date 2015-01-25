/*
 *  kernel.ld is a script for xos
 *
 * 首先声明内核程序的入口地址符号是"kernel_init"
 * 该脚本告诉GNU ld 如何构造内核镜像
 * 同时把调试信息也加载进来, 对应四个宏, 记录相关信息 (stab段中 stabstr段中)
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
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

    /* PROVIDE(etext = .) 表示程序中引用了etext,却没有定义它,那么链接器就是用链接器脚本中的定义  这个变量在kdebug中会使用*/
    PROVIDE(etext = .);

    .rodata : {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }

    /* 用于调试信息*/
    .stab : {
        /*__STAB_BEGIN__ 和__STAB_END__ 在kdebug中会使用*/
        PROVIDE(__STAB_BEGIN__ = .);
        *(.stab);
        PROVIDE(__STAB_END__ = .);
        BYTE(0)
    }

    .stabstr : {
        PROVIDE(__STABSTR_BEGIN__ = .);
        *(.stabstr);
        PROVIDE(__STABSTR_END__ = .);
        BYTE(0)
    }

    /* 采用4k页对齐*/
    . = ALIGN(0x1000);

    .data : {
        *(.data)
    }

    PROVIDE(edata = .);
    .bss :
    {
        *(.bss)
    }
    PROVIDE(end = .);

    /DISCARD/ : { *(.comment) *(.eh_frame)  }
}
