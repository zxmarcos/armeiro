#ifndef __KLIST_H
#define __KLIST_H

#include "base.h"

struct klist;
struct klist_node;

struct klist {
	struct klist_node *head;
	struct klist_node *tail;
	struct klist_node *current;
	u32 nodes;
};

struct klist_node {
	void *data;
	struct klist_node *next;
};

typedef struct klist_node klistnode_t;
typedef struct klist klist_t;

void klist_init(klist_t *kl);
u32 klist_next(klist_t *kl);
void klist_remove(klist_t *kl, void *data);
void klist_add_head(klist_t *kl, void *data);
void klist_add_tail(klist_t *kl, void *data);

static inline int klist_size(klist_t *kl) {
	return kl->nodes;
}

#endif
