
bin/kernel:     file format elf32-i386


Disassembly of section .text:

00100000 <main>:

int main(void)
{
  100000:	55                   	push   %ebp
  100001:	89 e5                	mov    %esp,%ebp
    return 0;
  100003:	b8 00 00 00 00       	mov    $0x0,%eax
}
  100008:	5d                   	pop    %ebp
  100009:	c3                   	ret    
