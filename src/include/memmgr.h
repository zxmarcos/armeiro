#ifndef __MEMMGR_H
#define __MEMMGR_H

#include "base.h"

/* Useful macros */
#define __va_to_pgdir(n)	((n & 0xfff00000) >> 20)
#define __va_to_coarse(n)	((n & 0x000ff000) >> 12)
#define __va_to_section(n)	(n & 0x000fffff)
#define __va_to_fine(n)		((n & 0x000ffc00) >> 10)

#define make_section(addr, ap, domain, c, b)	\
	(((addr) & 0xfff00000) | (((ap) & 3) << 10) | (((domain) & 0xf) << 5) |	\
	(((c) & 1) << 3) | (((b) & 1) << 2) | 0x12)

#define make_coarse(addr, domain)	\
	(((addr) & 0xfffffc00) | (((domain) & 0xf) << 5) | 0x11)

struct pgtbl_t { u32 val; };

/* coarse page table 256x4kb = 1MB */
struct pgcoarse_t { struct pgtbl_t *tbl[256]; };

/* translation table / page directory 32kb */
struct pgdir_t { void *pgtbl[4096]; };

void mm_init();
void *page_frame_alloc();
void page_frame_free(void *ptr);
void *kmalloc(size_t size);
void kfree(void *ptr);
void kmalloc_init();
void *page_alloc_align(u32 align, u32 pages);
void paging_init();

#endif