#include <kernel/types.h>
#include <kernel/mm.h>
#include <kernel/lock.h>
#include <asm/platform.h>
#include <queue.h>

void queue_init(queue_t *queue)
{
	queue->pending = 0;
	queue->head = NULL;
	queue->tail = NULL;
	lock_init(&queue->lock);
}

int queue_push(queue_t *queue, void *data)
{
	queuenode_t *node = kmalloc(sizeof(queuenode_t));
	if (!node) {
		printk("queue: out of memory!\n");
		return 1;
	}
	lock_acquire(&queue->lock);

	node->data = data;
	node->next = NULL;
	
	if (!queue->head && !queue->tail) {
		queue->head = node;
		queue->tail = node;
		goto _ok;
	}
	if (queue->tail) {
		queue->tail->next = node;
		queue->tail = node;
		goto _ok;
	} else {
		printk("queue: push, inconsistent queue!\n");
		kfree(node);
		lock_release(&queue->lock);
		return 1;
	}

 _ok:
	queue->pending++;
	//printk("queue: push %x, %d, head:%x,tail:%x\n", data, queue->pending, queue->head, queue->tail); 
	
	lock_release(&queue->lock);
	return 0;
}

void *queue_pop(queue_t *queue)
{
	if (!queue->pending) {
		return NULL;
	}
	if (!queue->head) {
		printk("queue: pop, inconsistent queue! %d\n", queue->pending);
		return NULL;
	}
	
	lock_acquire(&queue->lock);
	
	void *data = queue->head->data;
	queuenode_t *next = queue->head->next;
	kfree(queue->head);
	queue->head = next;
	if (!next)
		queue->tail = NULL;
	queue->pending--;
	
	//printk("queue: pop %x, %d, head:%x,tail:%x\n", data, queue->pending, queue->head, queue->tail); 
	
	lock_release(&queue->lock);
	
	return data;
}
