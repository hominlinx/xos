#ifndef __ELF_H__
#define __ELF_H__

#include <defs.h>

#define ELF_MAGIC 0x464C457FU // elf 的魔法数字

struct elfhdr {
    uint32_t e_magic; //必须等于ELF_MAGIC
    uint8_t e_elf[12];
    uint16_t e_type;      // 1=relocatable, 2=executable, 3=shared object, 4=core image
    uint16_t e_machine;   // 3=x86, 4=68K, etc.
    uint32_t e_version;   // file version, always 1
    uint32_t e_entry;     // entry point if executable 入口地址,ELF文件加载完成后程序控制的跳转地址, 这个只对可执行文件有意义，其它目标文件种类中，这个值为0, 即程序的虚拟地址入口点
    uint32_t e_phoff;     // file position of program header or 0 ,ELF文件中程序头表（program header table）的首地址位置。
    uint32_t e_shoff;     // file position of section header or 0, ELF文件的段头表（section header table）在文件中的位置
    uint32_t e_flags;     // architecture-specific flags, usually 0
    uint16_t e_ehsize;    // size of this elf header
    uint16_t e_phentsize; // size of an entry in program header
    uint16_t e_phnum;     // number of entries in program header or 0
    uint16_t e_shentsize; // size of an entry in section header
    uint16_t e_shnum;     // number of entries in section header or 0
    uint16_t e_shstrndx;  // section number that contains section name strings
};

/* program section header */
//程序头表
struct proghdr {
    uint32_t p_type;   // loadable code or data, dynamic linking info,etc.
    uint32_t p_offset; // file offset of segment
    uint32_t p_va;     // virtual address to map segment
    uint32_t p_pa;     // physical address, not used
    uint32_t p_filesz; // size of segment in file
    uint32_t p_memsz;  // size of segment in memory (bigger if contains bss）
    uint32_t p_flags;  // read/write/execute bits
    uint32_t p_align;  // required alignment, invariably hardware page size
};
#endif
