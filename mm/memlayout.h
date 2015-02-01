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
#define VPT                 0xFAC00000

#define KSTACKPAGE          2                           // # of pages in kernel stack
#define KSTACKSIZE          (KSTACKPAGE * PGSIZE)       // sizeof kernel stack


#endif /* !__KERN_MM_MEMLAYOUT_H__ */

