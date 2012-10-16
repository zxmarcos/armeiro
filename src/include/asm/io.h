#ifndef __IO_H
#define __IO_H

#include <kernel/types.h>

static inline
void __always_inline writel(u32 addr, u32 data)
{
	*((volatile u32 *) addr) = data;
}

static inline
void __always_inline writew(u32 addr, u32 data)
{
	*((volatile u16 *) addr) = (data & 0xffff);
}

static inline
void __always_inline writeb(u32 addr, u32 data)
{
	*((volatile u8 *) addr) = (data & 0xff);
}

static inline
u32 __always_inline readl(u32 addr)
{
	return *((volatile u32 *) addr);
}

static inline
u16 __always_inline readw(u32 addr)
{
	return *((volatile u16 *) addr);
}

static inline
u8 __always_inline readb(u32 addr)
{
	return *((volatile u8 *) addr);
}

#endif