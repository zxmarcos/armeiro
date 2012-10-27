#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include <kernel/types.h>
#include <kernel/lock.h>
#include <queue.h>

typedef struct semaphore_t {
	lock_t lock;
	int counter;
	queue_t wait;
} semaphore_t;

void sem_init(semaphore_t *sem, int icounter);
void sem_wait(semaphore_t *sem);
void sem_signal(semaphore_t *sem);

#endif