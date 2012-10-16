#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <kernel/types.h>

struct cpu_ctx {
	u32 cpsr;
	u32 sp;
	u32 lr;
	u32 pc;
	u32 r[13]; /* GPR */
} __packed;

#endif