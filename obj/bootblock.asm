
obj/bootblock.o:     file format elf32-i386


Disassembly of section .text:

00007c00 <start>:
.set CR0_PE_ON,             0x1                     # protected mode enable flag

.globl start
start:
.code16
    cli #关中断
    7c00:	fa                   	cli    
    cld #清方向标志位
    7c01:	fc                   	cld    

# set the DS ES SS zero
    xorw %ax, %ax
    7c02:	31 c0                	xor    %eax,%eax
    movw %ax, %ds
    7c04:	8e d8                	mov    %eax,%ds
    movw %ax, %es
    7c06:	8e c0                	mov    %eax,%es
    movw %ax, %ss
    7c08:	8e d0                	mov    %eax,%ss

00007c0a <seta20.1>:

#Enable A20
seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    7c0a:	e4 64                	in     $0x64,%al
    testb $0x2, %al
    7c0c:	a8 02                	test   $0x2,%al
    jnz seta20.1
    7c0e:	75 fa                	jne    7c0a <seta20.1>

    movb $0xd1, %al                                 # 0xd1 -> port 0x64
    7c10:	b0 d1                	mov    $0xd1,%al
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port
    7c12:	e6 64                	out    %al,$0x64

00007c14 <seta20.2>:

seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    7c14:	e4 64                	in     $0x64,%al
    testb $0x2, %al
    7c16:	a8 02                	test   $0x2,%al
    jnz seta20.2
    7c18:	75 fa                	jne    7c14 <seta20.2>

    movb $0xdf, %al                                 # 0xdf -> port 0x60
    7c1a:	b0 df                	mov    $0xdf,%al
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1
    7c1c:	e6 60                	out    %al,$0x60

#设置gdtr
    lgdt gdtdesc
    7c1e:	0f 01 16             	lgdtl  (%esi)
    7c21:	48                   	dec    %eax
    7c22:	7c 0f                	jl     7c33 <gdt+0x3>

#设置cr0
     movl %cr0, %eax
    7c24:	20 c0                	and    %al,%al
     orl $CR0_PE_ON, %eax
    7c26:	66 83 c8 01          	or     $0x1,%ax
     movl %eax, %cr0
    7c2a:	0f 22 c0             	mov    %eax,%cr0
    7c2d:	8d 74 00 00          	lea    0x0(%eax,%eax,1),%esi

00007c30 <gdt>:
	...
    7c38:	ff                   	(bad)  
    7c39:	ff 00                	incl   (%eax)
    7c3b:	00 00                	add    %al,(%eax)
    7c3d:	9a cf 00 ff ff 00 00 	lcall  $0x0,$0xffff00cf
    7c44:	00 92 cf 00 17 00    	add    %dl,0x1700cf(%edx)

00007c48 <gdtdesc>:
    7c48:	17                   	pop    %ss
    7c49:	00 30                	add    %dh,(%eax)
    7c4b:	7c 00                	jl     7c4d <gdtdesc+0x5>
	...

00007c4e <kernel_init>:

int kernel_init(void)
{
    7c4e:	55                   	push   %ebp
    return 0;
}
    7c4f:	31 c0                	xor    %eax,%eax

int kernel_init(void)
{
    7c51:	89 e5                	mov    %esp,%ebp
    return 0;
}
    7c53:	5d                   	pop    %ebp
    7c54:	c3                   	ret    
