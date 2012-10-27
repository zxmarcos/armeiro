/* simple scheduler
 *
 * Marcos Medeiros
 */


#include <kernel/task.h>
#include <kernel/scheduler.h>
#include <kernel/loopz.h>
#include <kernel/lock.h>
#include <kernel/mm.h>
#include <asm/cpu.h>
#include <asm/context.h>
#include <asm/platform.h>
#include <kernel/types.h>
#include <klist.h>
#include <printk.h>
#include <memory.h>

#define DEF_QUANTUM	1
#define PID_BASE	256

static u32 __kpidcounter = PID_BASE;
klist_t active_queue;
static u32 __prempt_enable = 0;

/* default idle task (kernel main) */
static struct task_ctx __idle_task = {
	.cpu = { 0 },
	.name = "idle_kernel",
	.pid = 0,
	.flags = TASK_RUNNABLE,
	.time_slice = DEF_QUANTUM,
};

static struct task_ctx *current_task = &__idle_task;

struct task_ctx *get_current_task()
{
	return current_task;
}

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
	task->flags = TASK_RUNNABLE;
	task->time_slice = DEF_QUANTUM;
	klist_add_tail(&active_queue, task);
}

static lock_t mylock = 0;

void task1()
{
	for (;;) {
		lock_acquire(&mylock);
		printk("1 acquired lock!\n");
		lock_release(&mylock);
		asm("swi #0");
	}
}

void task2()
{
	for (;;) {
		lock_acquire(&mylock);
		printk("2 acquired lock!\n");
		lock_release(&mylock);
		asm("swi #0");
	}
}


void task3()
{
	for (;;) {
		lock_acquire(&mylock);
		printk("3 acquired lock!\n");
		lock_release(&mylock);
		asm("swi #0");
	}
}

void task4()
{
	for (;;) {
		lock_acquire(&mylock);
		printk("4 acquired lock!\n");
		lock_release(&mylock);
		asm("swi #0");
	}
}

void scheduler_init()
{
	klist_init(&active_queue);
	klist_add_head(&active_queue, &__idle_task);

	lock_init(&mylock);
	scheduler_create_task(task1, NULL, 0);
	scheduler_create_task(task2, NULL, 0);
	scheduler_create_task(task3, NULL, 0);
	scheduler_create_task(task4, NULL, 0);
}

void prempt_disable()
{
	__prempt_enable = 0;
}

void prempt_enable()
{
	__prempt_enable = 1;
}

#define switch_to(prev, next) do {			\
		cpu_save_context(prev);				\
		current_task = next;				\
		cpu_switch_context(current_task);	\
	} while (0)


void do_prempt()
{
	if (current_task->time_slice && (current_task->flags & TASK_RUNNABLE)) {
		current_task->time_slice--;
		return;	
	} else {
		/* quantum expired */
		if ((current_task->flags & TASK_RUNNABLE) || (!current_task->time_slice))
			current_task->time_slice = DEF_QUANTUM;

		struct task_ctx *next_task = NULL;

		u32 ntask = 0;
		u32 ntask_max = klist_size(&active_queue);

		/* select next task to be prempted */
		for (;;) {
			if (ntask >= ntask_max)
				return;

			u32 ret = klist_next(&active_queue);
			/* only task */
			if (ret < 2)
				return;

			next_task = (struct task_ctx *) klist_entry(&active_queue);
			if (next_task->flags & TASK_RUNNABLE) {
				switch_to(current_task, next_task);
				printk("panic: we should never reach this point! ops...\n");
				for (;;);
			}
			ntask++;
		}
	}
}

void scheduler()
{
	if (__prempt_enable)
		do_prempt();	
}
