
#include "console.h"
#include "stdio.h"
#include "kdebug.h"
void kernel_init(void)
{
    cons_init();
    //cons_putc('c', rc_black, rc_green);
    cprintf("hominlinx %s", "and shelly");
    print_kerninfo();
    while(1);
}


