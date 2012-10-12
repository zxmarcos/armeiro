/*
 * Simple dynamic memory allocator
 * First Fit
 * Marcos Medeiros
 */
#include "platform.h"
#include "memmgr.h"

#define NODE_PTR(blk)	((u8 *) blk + sizeof(kmblock_t))
#define NEXT_NODE(blk)	\
 	(kmblock_t *)((u8 *) blk + sizeof(kmblock_t) + blk->size)
#define PTR_TO_NODE(ptr)	\
 	(kmblock_t *)((u8 *) ptr - sizeof(kmblock_t));

const u32 __heap_pages = 128; /* 128*4kb = 512kb */

static u32 heap_size = 0;
static u32 heap_alloc = 0; /* total memory allocated */
static u32 heap_blocks = 0; /* blocks created */

#define KM_USED	0x55534544
#define KM_FREE	0x46524545

typedef struct kmalloc_block {
	u32 magic;
	u32 size;
	u8 *start;
	struct kmalloc_block *next;
} kmblock_t;

kmblock_t *heap_start = NULL;

void kmalloc_init()
{
	heap_start = (kmblock_t *) page_frame_alloc();
	u32 i = 0;
	for (i = 1; i < __heap_pages; i++)
		page_frame_alloc();

	heap_size = PAGE_SIZE * __heap_pages;
	heap_alloc = 0;
	heap_blocks = 0;

	printk("Heap size: %d, kmblock_t: %d, base %x\n", heap_size,
		sizeof(kmblock_t), heap_start);
}


void *kmalloc(size_t size)
{
	if ((size + sizeof(kmblock_t)) >= (heap_size - heap_alloc))
		goto err_outofmem;

	kmblock_t *bk = heap_start;

	/* Create first block */
	if (!heap_blocks) {
		bk->magic = KM_USED;
		bk->size = size;
		bk->start = NODE_PTR(bk);
		bk->next = NEXT_NODE(bk);
		memclr(bk->next, sizeof(kmblock_t));
		heap_alloc += sizeof(kmblock_t) + size;
		heap_blocks++;

	} else {
		while (bk) {
			if (bk->magic == KM_FREE) {
				/* already allocated */
				if (bk->size >= size) {

					/* TODO: split block into smaller blocks */
					bk->magic = KM_USED;
					goto _ok;
				} 
				bk = bk->next;
				continue;

			} else
			if (bk->magic == KM_USED) {
				bk = bk->next;
				continue;
			} else {
				/* create new block */
				bk->magic = KM_USED;
				bk->size  = size;
				bk->start = NODE_PTR(bk);
				bk->next = NEXT_NODE(bk);
				memclr(bk->next, sizeof(kmblock_t));
				heap_alloc += sizeof(kmblock_t) + size;
				heap_blocks++;
				goto _ok;
			}
		}
		goto err_outofmem;
	}
_ok:
	//printk("kmalloc: %d size in %x\n", size, bk->start);
	return bk->start;

err_outofmem:
	printk("kmalloc: out of memory!\n");
	return NULL;
}

void kfree(void *ptr)
{
	if (!ptr)
		return;

	kmblock_t *bk = PTR_TO_NODE(ptr);
	if (bk->magic != KM_USED) {
		printk("kfree: invalid memory frame!\n");
		return;
	}
	bk->magic = KM_FREE;
	heap_alloc -= bk->size + sizeof(kmblock_t);
}