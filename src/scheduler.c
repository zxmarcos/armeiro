/* simple scheduler
 *
 * Marcos Medeiros
 */


#include <kernel/task.h>
#include <kernel/scheduler.h>
#include <kernel/mm.h>
#include <asm/cpu.h>
#include <asm/context.h>
#include <asm/platform.h>
#include <kernel/types.h>
#include <klist.h>
#include <printk.h>
#include <memory.h>

#define PID_BASE	256

static u32 __kpidcounter = PID_BASE;
klist_t active_queue;

/* default idle task (kernel main) */
static struct task_ctx __idle_task = {
	.cpu = { 0 },
	.name = "idle_kernel",
	.pid = 0,
	.flags = 0,
};

static struct task_ctx *current_task = &__idle_task;

void scheduler_create_task(void *f, void *arg, int user_mode)
{
	struct task_ctx *task = kmalloc(sizeof(struct task_ctx));
	if (!task) {
		printk("sched: can't create task!\n");
		return;
	}
	memclr(task, sizeof(struct task_ctx));
	
	u32 i = 0;

	for (i = 0; i < 13; i++)
		task->cpu.r[i] = 0xbeef0000 + i;
	task->cpu.r[0] = (u32) arg;
	/* allocate stack memory */
	task->cpu.sp = (u32) page_frame_alloc();
	task->cpu.pc = (u32) f;
	task->cpu.lr = (u32) f;
	if (user_mode)
		task->cpu.cpsr = 0x10;
	else
		task->cpu.cpsr = 0x1f;

	task->pid = __kpidcounter++;
	klist_add_tail(&active_queue, task);
}

void C_task1() {
	for (;;) {
		printk("task 1\n");
		cpu_delay(0x3ffffff);
	}
}
void C_task2() {
	for (;;) {
		printk("task 2\n");
		cpu_delay(0x3ffffff);
	}
}

void scheduler_init()
{
	klist_init(&active_queue);
	klist_add_head(&active_queue, &__idle_task);

	scheduler_create_task(C_task1, NULL, 1);
	scheduler_create_task(C_task2, NULL, 1);
}

void scheduler()
{
	/* Just one thread */
	if (klist_size(&active_queue) < 2)
		return;

	cpu_save_context(current_task);
	//cpu_dump(&current_task->cpu, CPU_SHOW_STATUS);
	
	/* select next task to be prempted */
	klist_next(&active_queue);

	current_task = active_queue.current->data;
	//printk("context_switch to %d\n", current_task->pid);
	cpu_switch_context(current_task);
}
