
#include "console.h"
#include "stdio.h"
void kernel_init(void)
{
    cons_init();
    //cons_putc('c', rc_black, rc_green);
    cprintf("hominlinx %s", "and shelly");
    while(1);
}
