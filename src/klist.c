/* Kernel linked lists
 * Marcos Medeiros
 */
#include "memmgr.h"
#include "klist.h"

void klist_init(klist_t *kl)
{
	if (!kl)
		return;

	kl->nodes = 0;
	kl->head = NULL;
	kl->tail = NULL;
	kl->current = NULL;
}

void klist_add_head(klist_t *kl, void *data)
{
	if (!kl)
		return;

	klistnode_t *node = kmalloc(sizeof(klistnode_t));
	if (!node)
		return;

	//printk("node: %x, %x\n", node, data);

	node->data = data;
	node->next = kl->head;
	if (!kl->nodes) {
		kl->tail = node;
		kl->current = node;
	}
	kl->head = node;
	kl->nodes++;
}

void klist_add_tail(klist_t *kl, void *data)
{
	if (!kl)
		return;

	klistnode_t *node = kmalloc(sizeof(klistnode_t));
	if (!node)
		return;

	//printk("node: %x, %x\n", node, data);
	node->data = data;
	node->next = NULL;
	if (!kl->nodes) {
		kl->head = node;
		kl->tail = node;
		kl->current = node;
	} else {
		kl->tail->next = node;
		kl->tail = node;
	}
	kl->nodes++;
}

u32 klist_next(klist_t *kl)
{
	if (!kl)
		return 0;
	if (!kl->nodes)
		return 0;

	if (!kl->current->next) {
		kl->current = kl->head;
	//	printk("node: %x, %x\n", kl->current->data);
		return 1;
	} else {
		kl->current = kl->current->next;
	//	printk("node: %x, %x\n", kl->current->data);
		return 2;
	}
}

void klist_remove(klist_t *kl, void *data)
{
	if (!kl)
		return;
	if (!kl->nodes)
		return;

	klistnode_t *node = kl->head;
	klistnode_t *last = NULL;

	while (node) {
		if (node->data == data) {
			/* first element */
			if (!last)
				kl->head = node->next;
			else
				last->next = node->next;
			kfree(node);
			kl->nodes--;
			return;
		}
		last = node;
		node = node->next;
	}
}
