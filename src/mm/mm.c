/*
 * Physical memory allocator
 * Bitmap based
 * Marcos Medeiros
 */

/*
 * pf - page frame
 * pfbb - page frame bitmap block
 */
#include <asm/io.h>
#include <asm/platform.h>
#include <kernel/mm.h>
#include <printk.h>
#include <memory.h>

#define BITMAP_PAGES	((MEMORY_SIZE / PAGE_SIZE) / 8)

static u8 pf_bitmap[BITMAP_PAGES];		/* 4kb frames */
static u32 __next_page = 0;
static const u32 total_frames = (MEMORY_SIZE / PAGE_SIZE);
static u32 n_alloc_pages = 0;

/*
 * Find first free page in whole pf_bitmap
 */
u32 pf_find_free()
{
	u32 page = 0;
	for (page = 0; page < BITMAP_PAGES; page++) {
		u8 blk_info = pf_bitmap[page];
		if (blk_info != 0xff) {
			u32 bit = 0;
			for (bit = 0; bit < 8; bit++) {
				if (!(blk_info & 0x80)) {
					return ((page << 3) | (bit & 7));
				}
				blk_info <<= 1;
			}
		}
	}
	return 0xffffffff;
}


u32 pf_state(u32 page)
{
	int pfblk_addr = 0;
	int pfblk_bit = 0;
	u8 blk_info = 0;

	if (page >= BITMAP_PAGES)
		return 0;

	pfblk_addr = page >> 3;
	pfblk_bit = 7 ^ (page & 7);

	blk_info = pf_bitmap[pfblk_addr];
	
	return (blk_info >> pfblk_bit) & 1;
}

/* 
 * Mark one page as free or allocated
 */
u32 pf_mark(u32 page, u32 is_free)
{
	int pfblk_addr = 0;
	int pfblk_bit = 0;
	u8 blk_info = 0;
	u32 result = 0;

	if (page >= BITMAP_PAGES)
		return result;

	pfblk_addr = page >> 3;
	pfblk_bit = 7 ^ (page & 7);

	blk_info = pf_bitmap[pfblk_addr];
	if (is_free) {
		/* Check if the page is allocated */
		if ((blk_info & (1 << pfblk_bit)))
			result = 1;

		/* If the page is at position less than current next free page
		 * then we set this as next free page.
		 */
		if (page < __next_page)
			__next_page = page;
		if (result) {
			blk_info &= ~(1 << pfblk_bit);
			pf_bitmap[pfblk_addr] = blk_info;
		}
	}
	else {
		/* Check if the page is free */
		if (!(blk_info & (1 << pfblk_bit)))
			result = 1;

		if (result) {
			blk_info |= (1 << pfblk_bit);
			pf_bitmap[pfblk_addr] = blk_info;
			/* If the current page that is being marked as allocated is the current
			 * next free page, then we find another next free page and mark
			 * this as allocated.
			 */
			if (page == __next_page)
				__next_page = pf_find_free();
		}
	}

	return result;
}



void *pf_alloc()
{
	/* Out of memory */
	if (__next_page == 0xffffffff)
		return NULL;

	void *ptr = (void *) (__next_page * PAGE_SIZE);
	if (pf_mark(__next_page, 0))
		n_alloc_pages++;

	return ptr;
}

/*
 * Slow =/
 */
void *page_alloc_align(u32 align, u32 pages)
{
	u32 pg = 0;
	u32 fit = 0;
	u32 res = 0;

	/* no alignment */
	if (align == 0)
		align = 1;

	for (; pg < total_frames; pg += align) {
		if (pf_state(pg))
			continue;
		res = 1;
		for (fit = 1; fit < pages; fit++) {
			if (pf_state(pg + fit)) {
				res = 0;
				break;
			}
		}
		if (res) {
			for (fit = 0; fit < pages; fit++)
				pf_mark(pg + fit, 0);
			return (void *) (pg * PAGE_SIZE);
		}
	}
	printk("page_alloc: can't allocate %d pages aligned to %d pages\n",
		pages, align);
	return NULL;
}

void *page_frame_alloc()
{
	return pf_alloc();
}

void pf_free(void *ptr)
{
	u32 addr = (u32) ptr;
	/* We can't free kernel pages */
	if (addr <= Kmap_end)
		return;
	addr >>= 12;
	if (pf_mark(addr, 1)) {
		if (n_alloc_pages)
			--n_alloc_pages;
	}
}

/*
 * Allocate kernel pages
 */
void pf_mark_kernel()
{
	u32 kpages = 0;
	u32 end = Kmap_end;
	u32 i = 0;
	/* Align end to page frame boundary 4kb */
	if (end & PAGE_SIZE_MSK) {
		end += PAGE_SIZE;
		end &= ~PAGE_SIZE_MSK;
	}
	
	kpages = (end / PAGE_SIZE);// >> 12;
	/* Just allocate first 'kpages' */
	for (i = 0; i < kpages; i++)
		pf_alloc();
}


void mm_init()
{
	memclr(pf_bitmap, BITMAP_PAGES);

	pf_mark_kernel();
	printk("memory bitmap size: %d\n", BITMAP_PAGES);
	printk("memory: frames %d/%d\n", total_frames - n_alloc_pages,
		total_frames);
	printk("\tkernel frames %x\n", n_alloc_pages);

	kmalloc_init();
	paging_init();

}


