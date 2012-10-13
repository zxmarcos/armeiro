/* Paging code
 * Marcos Medeiros
 */
#include "base.h"
#include "mmu.h"
#include "memmgr.h"
#include "platform.h"

#define KERNEL_DOMAIN	0

struct pgdir_t *__kernel_pgdir = NULL;

/* 4x4kb pages, 32kb aligned */
inline struct pgdir_t *pgdir_alloc()
{
	return page_alloc_align(8, 4);
}

int __section_map_identy(struct pgdir_t *dir, u32 start, u32 end)
{
	if (!dir)
		return -ERR_INVPARAM;

	u32 pages = 0;
	u32 page_addr = __va_to_pgdir(start);
	u32 __start = start;
	u32 __end = end;
	u32 i = 0;

	/* 1MB boundary */
	start >>= 20;
	end >>= 20;

	pages = end - start + 1;
	printk("%d sections needed to map 1x1 to %x:%x\n", pages, __start, __end);

	for (i = 0; i < pages; i++) {
		dir->pgtbl[page_addr] = (void *) make_section(page_addr << 20, KERNEL_DOMAIN, 0, 0, 0);
		page_addr++;
	}
	return -ERR_OK;
}

void setup_kernel_pgdir()
{
	__kernel_pgdir = pgdir_alloc();
	__section_map_identy(__kernel_pgdir, 0x00000000, MEMORY_SIZE);
	__section_map_identy(__kernel_pgdir, SYSTEM_REGION_START, SYSTEM_REGION_END);
	page_dir_set(__kernel_pgdir);
}

void paging_init()
{
	domain_set_protection(1);
	domain_setup(KERNEL_DOMAIN, DOMAIN_MANAGER);
	setup_kernel_pgdir();
	mmu_enable();
}

