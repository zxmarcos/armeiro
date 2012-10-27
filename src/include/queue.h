#ifndef __QUEUE_H
#define __QUEUE_H

#include <kernel/types.h>
#include <kernel/lock.h>

typedef struct queuenode_t {
	void *data;
	struct queuenode_t *next;
} queuenode_t;

typedef struct queue_t {
	queuenode_t *head;
	queuenode_t *tail;
	unsigned pending;
	lock_t lock;
} queue_t;

void queue_init(queue_t *queue);
int queue_push(queue_t *queue, void *data);
void *queue_pop(queue_t *queue);

#endif