/*
 * ARM CPU stuff
 * Marcos Medeiros
 */
#include <kernel/types.h>
#include <asm/cpu.h>
#include <asm/platform.h>
#include <kernel/task.h>
#include <memory.h>
#include <printk.h>

/* assembly level */
extern void __cpu_do_context_switch(struct cpu_ctx *ctx);
extern struct cpu_ctx *__cpu_saved_ctx;
extern u32 __cpu_is_save;

void show_stack_trace(struct cpu_ctx *cpu, u32 size)
{
	u32 *ptr = (u32 *) cpu->sp;
	u32 i = 0;
	for (i = 0; i < size; i++) {
		printk("%x: %x\n", ptr, *ptr);
		ptr++;
	}
}

void cpu_dump(struct cpu_ctx *ctx, u32 flags)
{
	if (flags & CPU_SHOW_GPR) {
		/* dump gpr */
		int out = 0;
		for (u32 i=0;i<13;i++) {
			printk("R%d: %x ", i, ctx->r[i]);
			out++;
			if (out == 5) {
				printk("\n");
				out = 0;
			}
		}
		if (out < 5)
			printk("\n");
	}

	if (flags & CPU_SHOW_STATUS) {
		/* dump cpu-state registers */
		printk("SP: %x  SR: %x\n", ctx->sp, ctx->cpsr);
		printk("PC: %x  LR: %x ", ctx->pc, ctx->lr);
		switch (ctx->cpsr & 0x1f) {
			case 0x1f: printk("sys\n"); break;
			case 0x10: printk("usr\n"); break;
			case 0x1b: printk("und\n"); break;
			case 0x17: printk("abt\n"); break;
			case 0x13: printk("svc\n"); break;
			case 0x11: printk("fiq\n"); break;
			case 0x12: printk("irq\n"); break;
			default:
				printk("invalid\n");
				break;
		}
	}
	if (flags & CPU_SHOW_STACK)
		show_stack_trace(ctx, 6);
}


void cpu_init()
{
	u32 info = __cpu_getid();
	u32 cache = __cpu_getcachet();
	u32 tcm = __cpu_gettcmst();
	printk("cpu info: arch: ARM%d, num:%x, rev:%d\n\tvar:%x, vendor:%x\n", CPUID_ARCH(info),
		CPUID_PARTNUM(info), CPUID_REV(info), CPUID_VENDOR(info));
	printk("\tcache type:%x, tcm status: %x\n", cache, tcm);
}

void cpu_save_context(struct task_ctx *task)
{
	/* Only save context if the cpu saved context before */
	if (__cpu_is_save)
		memcpy(&task->cpu, __cpu_saved_ctx, sizeof(struct cpu_ctx));
	else
		printk("cpu: no context saved!\n");
}

void cpu_switch_context(struct task_ctx *task)
{
	__cpu_is_save = 0;
	__cpu_do_context_switch(&task->cpu);
}
