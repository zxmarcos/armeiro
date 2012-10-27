/* ARM exceptions
 * Marcos Medeiros
 */
#include <asm/io.h>
#include <asm/cpu.h>
#include <asm/mmu.h>
#include <asm/platform.h>
#include <kernel/scheduler.h>
#include <kernel/task.h>

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
	(void) ctx;
	switch (id) {
		/* task reschedule */
		case 0:
			if (get_current_task()->flags & TASK_RUNNABLE)
				get_current_task()->time_slice = 0;
			scheduler();
			break;
	}
}