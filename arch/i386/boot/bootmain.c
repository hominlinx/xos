/*
 * boot_main 的目的是读取内核文件, 并且跳转到内核
 * 将内核加载到内存的起始地址 : 0x10000
 *
 */
#include <defs.h>
#include <x86.h>
#include <elf.h>

#define SECTSIZE 512 //一个扇区
#define ELFHDR   ((struct elfhdr*)0x10000)

/* waitdisk - wait for disk ready */
static void waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
//从ELF文件偏移为offset处，读取count个字节到内存地址为va处
static void readseg(uintptr_t va, uint32_t count, uint32_t offset) {
    //段的结束地址
    uintptr_t end_va = va + count;

    // round down to sector boundary
    //将va设置为512字节对齐的地方
    va -= offset % SECTSIZE;

    // translate from bytes to sectors; kernel starts at sector 1
    // 将相对于ELF文件头的偏移量转换为扇区，ELF格式的内核文件存放在第一个扇区中, secno 为扇区号
    uint32_t secno = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    // 开始读取该程序段的内容
    for (; va < end_va; va += SECTSIZE, secno ++) {
        //每次读取程序的一个节，即一个扇区, 也就是将offset扇区中的内容，读到物理地址为va的地方
        readsect((void *)va, secno);
    }
}

int boot_main(void)
{
    //read the first page off disk 读8个扇区
    ////将硬盘上从第一个扇区开始的4096个字节读到内存中地址为0x10000处
    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

    // is this a valid ELF?
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    //定义了两个程序头表项指针
    struct proghdr *ph, *eph;

    // load each program segment (ignores ph flags)
    //找到第一程序头表项的起始地址
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    //程序头表的结束位置
    eph = ph + ELFHDR->e_phnum;
    //将内核加载进入内存
    for (; ph < eph; ph ++) {
        //p_pa就是该程序段应该加载到内存中的位置
        //读取一个程序段的数据到内存中
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }

    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);

    return 0;
}
