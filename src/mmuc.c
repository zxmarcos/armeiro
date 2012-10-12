/* MMU stuff
 * Marcos Medeiros
 */
#include "base.h"
#include "mmu.h"
#include "platform.h"

void domain_set_protection(int present)
{
	u32 cr = __mmu_getcr();
	if (present)
		cr |= MMU_CR_SYS_PROT;
	else
		cr &= ~MMU_CR_SYS_PROT;
	__mmu_setcr(cr);
}

int domain_setup(u32 num, u32 flags)
{
	if (num >= 16) {
		printk("domain: invalid domain number!\n");
		return -ERR_INVDOMAIN;
	}

	u32 dom = __mmu_getdomain();
	dom = ((flags & 3) << (num << 1));
	__mmu_setdomain(dom);
	return -ERR_OK;
}

int page_dir_set(struct pgdir_t *dir)
{
	if (!dir) {
		printk("pgdir: invalid dir!\n");
		return -ERR_INVPGDIR;
	}
	__mmu_setttbl(dir);
	return -ERR_OK;
}

void mmu_enable()
{
	u32 cr = __mmu_getcr();
	cr |= MMU_CR_ENABLE;
	__mmu_setcr(cr);
}

void mmu_disable()
{
	u32 cr = __mmu_getcr();
	cr &= ~MMU_CR_ENABLE;
	__mmu_setcr(cr);
}