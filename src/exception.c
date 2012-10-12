/* ARM exceptions
 * Marcos Medeiros
 */
#include "io.h"
#include "cpu.h"
#include "mmu.h"
#include "platform.h"
#include "scheduler.h"

void __CPU_FIQ arm_fiq()
{
	printk("fiq\n");
}

void __CPU_IRQ arm_irq()
{
}

void __CPU_ABT arm_abt()
{
	mmu_disable();
	printk("page_fault error: %bb at %x\n", __mmu_getfsr(), __mmu_getfar());
	for (;;);
}

void syscall_handler(struct cpu_ctx *ctx, u32 id)
{
	printk("syscall %d from %x\n", id, ctx->pc);
}