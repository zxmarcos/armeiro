#ifndef __ARM_CPU_H
#define __ARM_CPU_H

#include <kernel/types.h>
#include <kernel/task.h>
#include <asm/context.h>

#define __CPU_IRQ	__attribute__((interrupt("IRQ")))
#define __CPU_SWI	__attribute__((interrupt("SWI")))
#define __CPU_FIQ	__attribute__((interrupt("FIQ")))
#define __CPU_ABT	__attribute__((interrupt("ABT")))

struct task_ctx;

void cpu_init();
void cpu_save_context(struct task_ctx *task);
void cpu_switch_context(struct task_ctx *task);

#define CPU_SHOW_GPR	0x01
#define CPU_SHOW_STATUS	0x02
#define CPU_SHOW_STACK	0x04
void cpu_dump(struct cpu_ctx *ctx, u32 flags);

#define CPUID_REV(n)		(n & 0xf)
#define CPUID_PARTNUM(n)	((n >> 4) & 0xfff)
#define CPUID_ARCH(n)		((n >> 16) & 0xf)
#define CPUID_VAR(n)		((n >> 20) & 0xf)
#define CPUID_VENDOR(n)		((n >> 24) & 0xff)

extern u32 __cpu_getid();
extern u32 __cpu_getcachet();
extern u32 __cpu_gettcmst();
extern void cpu_delay(u32 n);

#endif