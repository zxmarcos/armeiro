/*  semaphores
 * Marcos Medeiros
 */
#include <kernel/types.h>
#include <kernel/lock.h>
#include <kernel/task.h>
#include <kernel/semaphore.h>
#include <asm/atomic.h>
#include <printk.h>
#include <errno.h>
#include <queue.h>

#define SEM_DEBUG	1

void sem_init(semaphore_t *sem, int icounter)
{
	lock_init(&sem->lock);
	sem->counter = icounter;
	queue_init(&sem->wait);
}

void sem_wait(semaphore_t *sem)
{
	lock_acquire(&sem->lock);
	
	struct task_ctx *task = get_current_task();
	/* semaphore acquired */
	if (sem->counter > 0) {
		atomic_dec(&sem->counter);
		lock_release(&sem->lock);
#if SEM_DEBUG
		printk("semaphore acquired by %d!\n", task->pid);
#endif
		return;
	}
#if SEM_DEBUG
	printk("blocking task %d\n", task->pid);
#endif
	/* wait and block */
	queue_push(&sem->wait, task);
	task->flags &= ~TASK_RUNNABLE;
	lock_release(&sem->lock);
	/* reschedule */
	asm("swi #0");
	asm("swi #0");
#if SEM_DEBUG
	printk("semaphore acquired by %d after waiting!\n", task->pid);
#endif
}

void sem_signal(semaphore_t *sem)
{
	lock_acquire(&sem->lock);
	struct task_ctx *task = get_current_task();

#if SEM_DEBUG
	printk("task %d release semaphore.\n", task->pid);
#endif

	if ((int) atomic_inc(&sem->counter) >= 0) {
		/* wakeup first waiting task */
		task = queue_pop(&sem->wait);
		if (task) {
			atomic_dec(&sem->counter);
			task->flags |= TASK_RUNNABLE;
#if SEM_DEBUG
			printk("wakeup task %d\n", task->pid);
#endif
		}
	}
	lock_release(&sem->lock);
}
