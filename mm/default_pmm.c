#include <default_pmm.h>
#include <list.h>
#include <string.h>

free_area_t free_area;

#define free_list (free_area.free_list)
#define nr_free (free_area.nr_free)

static void default_init() {
    list_init(&free_list);
    nr_free = 0;
}

static void default_init_memmap(struct Page *base, size_t n) {
    assert(n > 0);
    struct Page *p = base; //估计base 是：0xc011924c，也就是pa[0x1b7000]对应的页．
    for (; p != base + n; ++p) {
        assert(PageReserved(p));
        p->flags = 0;
        SetPageProperty(p);
        p->property = 0;
        set_page_ref(p, 0);
        list_add_before(&free_list, &(p->page_link));
    }
    //for循环完成后，free_list成为了这个链表的头，此刻有ｎ个节点
    nr_free += n;
    base->property = n;
}

static struct Page *default_alloc_pages(size_t n) {
    assert(n > 0);
    if (n > nr_free) {
        return NULL;
    }
    return NULL;
}

static void default_free_pages(struct Page *base, size_t n) {
    
}

static size_t default_nr_free_pages(void) {
    return nr_free;
}

static void basic_check(void) {
    
}

static void default_check() {

}

const struct pmm_manager default_pmm_manager = {
    .name = "default_pmm_manager",
    .init = default_init,
    .init_memmap = default_init_memmap,
    .alloc_pages = default_alloc_pages,
    .free_pages = default_free_pages,
    .num_free_pages = default_nr_free_pages,
    .check = default_check,
};
