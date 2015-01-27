
#include "console.h"
#include "stdio.h"
#include "kdebug.h"

//函数声明
void kernel_init(void) __attribute__((noreturn)); ////函数不会返回。
void grade_backtrace(void);

void kernel_init(void)
{
    cons_init();
    //cons_putc('c', rc_black, rc_green);
    print_kerninfo();
    //grade_backtrace();
    //初始化物理内存管理
    pmm_init();
    //中断初始化
    pic_init();
    cprintf("hominlinx %s", "and shelly");
    while(1);
}

//函数不能作为inline函数优化
void __attribute__((noinline))
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) {
    mon_backtrace(0, NULL, NULL);
}

void __attribute__((noinline))
grade_backtrace1(int arg0, int arg1) {
    grade_backtrace2(arg0, (int)&arg0, arg1, (int)&arg1);
}

void __attribute__((noinline))
grade_backtrace0(int arg0, int arg1, int arg2) {
    grade_backtrace1(arg0, arg2);
}

void
grade_backtrace(void) {
    grade_backtrace0(0, (int)kernel_init, 0xffff0000);
}


