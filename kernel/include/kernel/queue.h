#ifndef KERNEL_ARCH_X86_64_QUEUE_H_
#define KERNEL_ARCH_X86_64_QUEUE_H_

#include <stdint.h>
#include <kernel/kerr.h>
#include "test.h"

typedef struct dequeue dequeue;
typedef struct dq_node dq_node;

struct dequeue {
  dq_node* head;
  dq_node* tail;
  uint32_t size;
};

struct dq_node {
  dq_node* prev;
  dq_node* next;
  dequeue* queue;
};

dq_node* dq_add_head(dequeue* queue, dq_node* node);
dq_node* dq_add_tail(dequeue* queue, dq_node* node);
dq_node* dq_add_before(dq_node* ref_node, dq_node* node);
dq_node* dq_add_after(dq_node* ref_node, dq_node* node);
dq_node* dq_remove_head(dequeue* queue);
dq_node* dq_remove_tail(dequeue* queue);
dq_node* dq_remove(dq_node* node);
void dq_clear(dequeue* queue);

SUITEDECL(dequeue);

#endif //KERNEL_ARCH_X86_64_QUEUE_H_
