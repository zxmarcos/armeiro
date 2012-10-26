#ifndef __ATOMIC_H
#define __ATOMIC_H

#include <kernel/types.h>

u32 atomic_xchg(void *ptr, u32 val);
u32 atomic_inc(void *ptr);
u32 atomic_dec(void *ptr);

#endif
