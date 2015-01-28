#ifndef __KERNEL_DRIVER_PICIRQ_H__
#define __KERNEL_DRIVER_PICIRQ_H__
#include "defs.h"
//
//初始化pic
void pic_init(void);
//使能中断
void pic_enable(uint32_t irq);

//如今绝大多数的PC都拥有两个8259A，这样最多可以接收15个中断源
//8259A的编程是通过向其相应的端口发送一系列的ICW（初始化命令字）完成的。
//总共需要发送四个ICW，它们都分别有自己独特的格式，而且必须按次序发送，并且必须发送到相应的端口，主片的端口号为0x20，从片的端口号为0xA0。
// 8259A的初始化是通过CPU对8259A送4个方式控制字ICW1、ICW2、ICW3、ICW4以及3个操作命令字OCW1、OCW2和OCW3。
//
// 由于8259A只占用两个I/O端口地址，但要写入四个方式控制字和三个操作命令字，
// 因而其各寄存器的读写是I/O地址和特征位及顺序配合完成的：写入方式控制字必须按照ICW1～ICW4的顺序进行。
#define IRQ_OFFSET 32
#endif
