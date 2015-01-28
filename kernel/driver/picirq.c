#include "picirq.h"
#include "x86.h"

// I/O Addresses of 8259a
// 外设的所有中断由中断控制芯片8259A统一汇集之后连接到CPU的INTR引脚 , 参见p19页 或p163页
// 0~31号中断是CPU 使用和保留的, 所以IRQ0对应的中断号为32, 参见p165页
#define IO_PIC1 0x20 //主片端口 0x20 0x21
#define IO_PIC2 0xA0 //从片端口 0xA0 0xA1
#define IRQ_SLAVE           2       // IRQ at which slave connects to master , 从片通过IRQ2与主片链接

//将IRQ2置零
static uint16_t irq_mask = 0xFFFF & ~(1 << IRQ_SLAVE);
static bool bInit = 0;

static void pic_setmask(uint16_t mask) {
    irq_mask = mask;
    if(bInit) {
        outb(IO_PIC1 + 1, mask);
        outb(IO_PIC2 + 1, mask >> 8);
    }
}

void pic_enable(uint32_t irq) {
    pic_setmask(irq_mask & ~(1 << irq));
}

//必须根据顺序初始化
void pic_init(void) {
    bInit = 1;
    //屏蔽所有中断
    outb(IO_PIC1 + 1, 0xFF);
    outb(IO_PIC2 + 1, 0xFF);

    //ICW1 :主要用来设定中断请求的触发方式，是否级联，初始化过程中用不用ICW4等。 A0 = 0
    // ICW1:  0001g0hi
    //    g:  0 = edge triggering, 1 = level triggering , 电平触发, 边沿触发
    //    h:  0 = cascaded PICs, 1 = master only , 1为联机
    //    i:  0 = no ICW4, 1 = ICW4 required
    outb(IO_PIC1 , 0x11);//1011

    //ICW2设置中断类型号, 低三位组成IR0~IR7, A0 = 1, CPU 自动插入低三位
    outb(IO_PIC1 + 1, IRQ_OFFSET);//0xff

    //ICW3 级联命令字
    outb(IO_PIC1 + 1, 1 << IRQ_SLAVE);

    // ICW4:  000nbmap
    // //    n:  1 = special fully nested mode
    // //    b:  1 = buffered mode
    // //    m:  0 = slave PIC, 1 = master PIC
    // //        (ignored when b is 0, as the master/slave role
    // //         can be hardwired).
    // //    a:  1 = Automatic EOI mode , 中断结束方式
    // //    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
    outb(IO_PIC1 + 1, 0x3); //0011

    // 设置 从片
    outb(IO_PIC2, 0x11);
    outb(IO_PIC2+1, IRQ_OFFSET + 8);
    outb(IO_PIC2+1, IRQ_SLAVE);
    outb(IO_PIC2 + 1, 0x3);


    //初始化后写入
    //工作方式编程 ocw2 ocw3 为偶地址A0 = 0 , 即 0x20 或0xA0
    //屏蔽方式, 级联时用
    // OCW3:  0ef01prs
    //   ef:  0x = NOP, 10 = clear specific mask, 11 = set specific mask
    //    p:  0 = no polling, 1 = polling mode
    //   rs:  0x = NOP, 10 = read IRR, 11 = read ISR
    outb(IO_PIC1, 0x68);    // clear specific mask
    outb(IO_PIC1, 0x0a);    // read IRR by default

    outb(IO_PIC2, 0x68);    // OCW3
    outb(IO_PIC2, 0x0a);    // OCW3

    //OCW1是中断屏蔽字 , 0 允许中断
    if (irq_mask != 0xFFFF) {
        pic_setmask(irq_mask);
    }

}
