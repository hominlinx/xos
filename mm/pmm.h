#ifndef __KERN_MM_PMM_H__
#define __KERN_MM_PMM_H__

/*
 * 1.如何建立页目录 页表
 * ２．页的映射关系是啥
 */
#include <defs.h>
#include <mmu.h>
#include <memlayout.h>
#include <assert.h>

//pmm_manager 物理存储控制模块，回调
struct pmm_manager {
    const int8_t *name; //XX_pmm_manager name
    void (*init)(void); 
    void (*init_memmap)(struct Page *base, size_t n);
    struct Page *(*alloc_pages)(size_t n);
    void (*free_pages)(struct Page *base, size_t n);
    size_t (*num_free_pages)(void);
    void (*check)(void);
};

extern const struct pmm_manager *pmm_manager;
extern pde_t *boot_pgdir;
extern uintptr_t boot_cr3;
extern struct Page *pages;
extern size_t npage;
extern int8_t bootstack[], bootstacktop[];

void pmm_init(void);

struct Page *alloc_pages(size_t n);
void free_pages(struct Page *base, size_t n);
size_t num_free_pages(void);

#define alloc_page() alloc_pages(1)
#define free_page(page) free_pages(page, 1)

//pte_t, pde_t指向的是物理空间，本身存在虚拟空间中
//返回的虚拟地址指向的二级页表项
pte_t *get_pte(pde_t *pgdir, uintptr_t la, bool create);
struct Page *get_page(pde_t *pgdir, uintptr_t la, pte_t **ptep_store);
void page_remove(pde_t *pgdir, uintptr_t la);
int page_insert(pde_t *pgdir, struct Page *page, uintptr_t la, uint32_t perm);

void load_esp0(uintptr_t esp0);
void tlb_invalidate(pde_t *pgdir, uintptr_t la);
void print_pgdir(void);


/*
 *PADDR 内核虚拟地址映射到物理地址, 这个是在执行页机制之前
 */
#define PADDR(kva) ({           \
    uintptr_t __m_kva = (uintptr_t)(kva);        \
    if (__m_kva < KERNBASE) { \
        panic("PADDR called with invalid kva: %p", __m_kva); \
    } \
    __m_kva - KERNBASE; \
    })

/*
 * KADDR 物理地址映射到虚拟地址
 */
#define KADDR(pa) ({ \
    uintptr_t __m_pa = (uintptr_t)(pa); \
    size_t __m_ppn = PPN(__m_pa);   \
    if (__m_ppn >= npage) { \
        panic("KADDR called with invalid pa: %p", __m_pa);  \
    }   \
    (void*)(__m_pa + KERNBASE); \
    })

extern struct Page *pages; //pages指向了物理页的首页
extern size_t npage; //一共多少个物理页

//从一个页得到对应的一个物理地址, 这个ｐａｇｅ页与ｐａｇｅｓ相减是sizeof(struct Page)的整数倍．
static inline physaddr_t page2pa(struct Page *page) {
    return (page - pages) << PGSHIFT;
}

//计算一个物理地址对应的页面
//比如pa = [0x1b7000], PPN(pa) = 0x1b7, 这个值除以sizeof(struct Page) 就是距离pages（0xC0117000)的大小，所以得到Page* 是0xc011924c
static inline struct Page* pa2page(physaddr_t pa) {
    if(PPN(pa) >= npage) {
        panic("pa2page called with invalid pa");
    }
    //cprintf("pa2page:pa[%p] PPN[%p] page[%p] pa[%p]\n", pa, PPN(pa), &pages[PPN(pa)], page2pa(&pages[PPN(pa)]));
    return &pages[PPN(pa)];
}

//把一个页面转换成虚拟地址，这个是在页机制之前
static inline void * page2kva(struct Page *page) {
    return KADDR(page2pa(page));
}

static inline struct Page *pte2page(pte_t pte) {
    if (!(pte & PTE_P)) {
        panic("pte2page called with invalid pte");
    }
    return pa2page(PTE_ADDR(pte));
}

static inline struct Page *
pde2page(pde_t pde) {
    return pa2page(PDE_ADDR(pde));
}

static inline int
page_ref(struct Page *page) {
    return page->ref;
}

static inline void
set_page_ref(struct Page *page, int val) {
    page->ref = val;
}

static inline int
page_ref_inc(struct Page *page) {
    page->ref += 1;
    return page->ref;
}

static inline int
page_ref_dec(struct Page *page) {
    page->ref -= 1;
    return page->ref;
}


#endif /* !__KERN_MM_PMM_H__ */

