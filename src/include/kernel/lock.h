#ifndef __LOCK_H
#define __LOCK_H

#include <asm/atomic.h>
#include <kernel/types.h>

typedef unsigned int lock_t;

static inline void __always_inline lock_init(lock_t *lock) {
	*lock = 0;
}

static inline void __always_inline lock_acquire(lock_t *lock) {
	u32 ret = 0;
	do {
		ret = atomic_xchg(lock, 1);
	} while (ret == 1);
}

static inline void __always_inline lock_release(lock_t *lock) {
	atomic_xchg(lock, 0);
}

#endif
