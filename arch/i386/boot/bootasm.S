#include "asm.h"
#start the CPU
# %cs = 0 ; %ip = 7c00


#实模式下物理地址 = CS << 4 + addr，保护模式下线性地址 = G/LDT[CS].base + addr
#内核代码段选择子
.set PROT_MODE_CSEG,        0x8                     # kernel code segment selector
#内核数据段选择子
.set PROT_MODE_DSEG,        0x10                    # kernel data segment selector
#保护模式使能标志
.set CR0_PE_ON,             0x1                     # protected mode enable flag

.globl start
start:
#CPU刚启动为16位模式 实模式
.code16
    cli #关中断
    cld #清方向标志位

# set the DS ES SS zero
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

#打开A20地址线
#Enable A20
seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.1

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port

seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.2

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1

    #设置gdtr
    #将全局描述符表描述符加载到全局描述符表寄存器
    # gdtr中有48位
    lgdt gdtdesc

    #cr0中的第0位为1表示处于保护模式
    #cr0中的第0位为0，表示处于实模式
    #把控制寄存器cr0加载到eax中
    #设置cr0 控制寄存器 设置PE位为1 则开启了保护模式, 默认为段级保护
     movl %cr0, %eax
     orl $CR0_PE_ON, %eax
     movl %eax, %cr0

    #跳转到32位模式中的下一条指令
    #将处理器切换为32位工作模式
    #下面这条指令执行的结果会将$PROT_MODE_CSEG加载到cs中，cs对应的高速缓冲存储器会加载代码段描述符 同样将$protcseg加载到ip中
    ljmp $PROT_MODE_CSEG, $protcseg
    #call kernel_init


.code32
protcseg:
    #设置保护模式下的数据寄存器
    #将数据段的选择子装入ax中 选择子参见赵炯书93页
    #将ax装入到其他数据段寄存器中，在装入的同时，
    #数据段描述符会自动的加入到这些段寄存器对应的高速缓冲寄存器中
    #为了快速方便地找到段选择符，处理器提供段寄存器 段寄存器地唯一目的是存放段选择符
    movw    $PROT_MODE_DSEG, %ax
    movw    %ax, %ds                # -> DS: Data Segment 数据段寄存器 指向包含静态数据或者全局数据段
    movw    %ax, %es                # -> ES: Extra Segment 附加段寄存器
    movw    %ax, %fs                # -> FS
    movw    %ax, %gs                # -> GS
    movw    %ax, %ss                # -> SS: Stack Segment 堆栈段寄存器 指向包含当前程序栈的段
    #设置栈指针，并且调用c函数
    movl $start, %esp
    call boot_main

 spin:
      jmp spin

#gdt
.p2align 2
gdt:
    SEG_NULLASM
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt

#以上描述了3个段, 每个段为8字节,称为段描述符, 这8个字符含有3个字段:段基地址,段限长, 段属性 参见:95页
#.word 0x0000,0x0000;
#.byte 0x00,0x00,0x00,0x00

#.word 0xFFFF,0x0000;
#.byte 0x00,0x9A,0xCF,0x00

#.word 0xFFFF,0x0000;
#.byte 0x00,0x92,0xCF,0x00




