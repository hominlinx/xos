#include "asm.h"
#start the CPU
# %cs = 0 ; %ip = 7c00

.set PROT_MODE_CSEG,        0x8                     # kernel code segment selector
.set PROT_MODE_DSEG,        0x10                    # kernel data segment selector
.set CR0_PE_ON,             0x1                     # protected mode enable flag

.globl start
start:
.code16
    cli #关中断
    cld #清方向标志位

# set the DS ES SS zero
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

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
    lgdt gdtdesc

#设置cr0
     movl %cr0, %eax
     orl $CR0_PE_ON, %eax
     movl %eax, %cr0

    #call kernel_init

#gdt
.p2align 2
gdt:
    SEG_NULLASM
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt

#.word 0x0000,0x0000;
#.byte 0x00,0x00,0x00,0x00
#.word 0xFFFF,0x0000;
#.byte 0x00,0x9A,0xCF,0x00
#.word 0xFFFF,0x0000;
#.byte 0x00,0x92,0xCF,0x00




