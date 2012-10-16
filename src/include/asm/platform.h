
#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <kernel/types.h>

/* 64MB */
#define MEMORY_SIZE	(0x4000000)
#define SYSTEM_REGION_START	0x10000000
#define SYSTEM_REGION_END	0x101f4000
#define PAGE_SIZE	0x1000 /*4k*/
#define PAGE_SIZE_MSK	(PAGE_SIZE - 1)

void timer_init();
void timer_irq_clear();
void printk(const char *fmt, ...);

extern u32 Kmap_start;
extern u32 Kmap_addr;
extern u32 Kmap_end;
extern u32 Kmap_sp_svc;
extern u32 Kmap_sp_irq;
extern u32 Kmap_sp_abt;

#endif