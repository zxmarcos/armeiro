#ifndef __MMU_H
#define __MMU_H

#include <kernel/types.h>
#include <kernel/mm.h>

/* assembly level */
u32 __mmu_getttbl();
u32 __mmu_getfsr();
u32 __mmu_getfar();
void __mmu_setttbl(void *base);
u32 __mmu_getdomain();
void __mmu_setdomain(u32 value);
u32 __mmu_getcr();
void __mmu_setcr(u32 value);

/* domain type */
#define DOMAIN_NOACESS	0x00
#define DOMAIN_CLIENT	0x01
#define DOMAIN_MANAGER	0x03
/* mmu control register */
#define MMU_CR_ENABLE		0x0000001
#define MMU_CR_ALIGN_FAULT	0x0000002
#define MMU_CR_DCACHE		0x0000004
#define MMU_CR_SYS_PROT		0x0000100
#define MMU_CR_ROM_PROT		0x0000200

void domain_set_protection(int present);
int domain_setup(u32 num, u32 flags);
int page_dir_set(struct pgdir_t *dir);
void mmu_enable();
void mmu_disable();


#endif