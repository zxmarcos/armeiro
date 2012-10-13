#ifndef __TASK_H
#define __TASK_H

#include "base.h"
#include "cpu.h"
#include "memmgr.h"

#define TASK_RUNNABLE	1

struct task_ctx {
	struct cpu_ctx cpu; /* ALWAYS the first item */
	const char *name;
	u32 pid;
	u32 flags;
};

#endif