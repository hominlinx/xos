#ifndef __KERN_MM_MEMLAYOUT_H__
#define __KERN_MM_MEMLAYOUT_H__

/* This file contains the definitions for memory management in our OS. */

/* global segment number */
#define SEG_KTEXT    1
#define SEG_KDATA    2
#define SEG_UTEXT    3
#define SEG_UDATA    4
#define SEG_TSS        5

/* global descrptor numbers */
#define GD_KTEXT    ((SEG_KTEXT) << 3)        // kernel text 1000 -->0x08
#define GD_KDATA    ((SEG_KDATA) << 3)
#define GD_UTEXT    ((SEG_UTEXT) << 3)        // user text 11000 --> 0x18
#define GD_UDATA    ((SEG_UDATA) << 3)        // user data 100000-->0x20
#define GD_TSS        ((SEG_TSS) << 3)        // task segment selector

//特权级字段
#define DPL_KERNEL    (0)
#define DPL_USER    (3)

#define KERNEL_CS    ((GD_KTEXT) | DPL_KERNEL) //0x08
#define KERNEL_DS    ((GD_KDATA) | DPL_KERNEL) //0x10
#define USER_CS        ((GD_UTEXT) | DPL_USER) //0x18 | 0x3 = 0x1B
#define USER_DS        ((GD_UDATA) | DPL_USER) //0x20 | 0x3 = 0x23
/* *
 * Virtual memory map:                                          Permissions
 *                                                              kernel/user
 *
 *     4G ------------------> +---------------------------------+
 *                            |                                 |
 *                            |         Empty Memory (*)        |
 *                            |                                 |
 *                            +---------------------------------+ 0xFB000000
 *                            |   Cur. Page Table (Kern, RW)    | RW/-- PTSIZE
 *     VPT -----------------> +---------------------------------+ 0xFAC00000
 *                            |        Invalid Memory (*)       | --/--
 *     KERNTOP -------------> +---------------------------------+ 0xF8000000
 *                            |                                 |
 *                            |    Remapped Physical Memory     | RW/-- KMEMSIZE
 *                            |                                 |
 *     KERNBASE ------------> +---------------------------------+ 0xC0000000
 *                            |                                 |
 *                            |                                 |
 *                            |                                 |
 *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.
 *
 * */

/* All physical memory mapped at this address */
#define KERNBASE            0xC0000000
#define KMEMSIZE            0x38000000                  // the maximum amount of physical memory
#define KERNTOP             (KERNBASE + KMEMSIZE)

/* *
 * Virtual page table. Entry PDX[VPT] in the PD (Page Directory) contains
 * a pointer to the page directory itself, thereby turning the PD into a page
 * table, which maps all the PTEs (Page Table Entry) containing the page mappings
 * for the entire virtual address space into that 4 Meg region starting at VPT.
 * */
//用户虚拟页表Virtual Page Table，大小为PTSIZE，这个是内核使用的
#define VPT                 0xFAC00000

#define KSTACKPAGE          2                           // # of pages in kernel stack
#define KSTACKSIZE          (KSTACKPAGE * PGSIZE)       // sizeof kernel stack

#ifndef __ASSEMBLER__
#include <defs.h>
#include <list.h>
#include <atomic.h>

//page table entry
typedef uintptr_t pte_t;
//page directory entry
typedef uintptr_t pde_t;

//some constants for bios interrupt 15h AX = 0xE820
//
//
#define E820MAX   20 //max num of entries in E820MAP
#define E820_ARM 1
#define E820_ARR 2
/*
 * nr_map 是内存段的数量
 * 每个内存段由e820entry 表示
 * addr 表示内存段的起始地址
 * size表示内存段的大小
 * type表示内存段的类型, E820_ARM 表示可用内存, E820MAX是一个宏，为20，说明最多可以有20个内存段
 */
struct e820map {
    uint32_t nr_map;
    struct e820entry {
        uint64_t addr;
        uint64_t size;
        uint32_t type;
    } __attribute__((packed)) map[E820MAX];
};

/*
 * Page结构体，　描述物理页，也叫页帧，现在每页是４ｋ
 */
struct Page {
    int ref; //页面被引用的次数
    uint32_t flags; //
    uint32_t property; //the number of free block;不是每个页面都用这个属性
    list_entry_t page_link; //free list link
};

/* Flags describing the status of a page frame */
//bit 0表示此页是否被保留(reserved),如果是被保留的页,则
//bit 0会设置为1,且不能放到空闲页链表中,即这样的页不是空闲页,不能动态分配与释放。比如目前内核代码占用的空间就
//属于这样“被保留”的页。在本实验中,bit 1表示此页是否是free的,如果设置为1,表示这页是free的,可以被分配;如果设
//置为0,表示这页已经被分配出去了,不能被再二次分配。
#define PG_reserved                 0       // the page descriptor is reserved for kernel or unusable
#define PG_property                 1       // the member 'property' is valid

#define SetPageReserved(page)       set_bit(PG_reserved, &((page)->flags))
#define ClearPageReserved(page)     clear_bit(PG_reserved, &((page)->flags))
#define PageReserved(page)          test_bit(PG_reserved, &((page)->flags))
#define SetPageProperty(page)       set_bit(PG_property, &((page)->flags))
#define ClearPageProperty(page)     clear_bit(PG_property, &((page)->flags))
#define PageProperty(page)          test_bit(PG_property, &((page)->flags))

// convert list entry to page
#define le2page(le, member)                 \
    to_struct((le), struct Page, member)

/* free_area_t - maintains a doubly linked list to record free (unused) pages */
typedef struct {
    list_entry_t free_list;         // the list header
    unsigned int nr_free;           // # of free pages in this free list
} free_area_t;

#endif

#endif /* !__KERN_MM_MEMLAYOUT_H__ */

