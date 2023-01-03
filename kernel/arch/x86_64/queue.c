#include <kernel/queue.h>
#include <stddef.h>
#include <stdio.h>
#include <test.h>
#include <assert.h>

static inline void dq_link(dq_node* node1, dq_node* node2) {
  node1->next = node2;
  node2->prev = node1;
}

static inline void dq_add_only(dequeue* queue, dq_node* node) {
  queue->head = node;
  queue->tail = node;
  node->prev = NULL;
  node->next = NULL;
  node->queue = queue;
  queue->size = 1;
}

dq_node* dq_add_head(dequeue* queue, dq_node* node) {
  if (queue->size == 0) {
    dq_add_only(queue, node);
  } else {
    dq_link(node, queue->head);
    queue->head = node;
    node->queue = queue;
    node->prev = NULL;
    queue->size++;
  }
  return node;
}

dq_node* dq_add_tail(dequeue* queue, dq_node* node) {
  if (queue->size == 0) {
    dq_add_only(queue, node);
  } else {
    dq_link(queue->tail, node);
    queue->tail = node;
    node->queue = queue;
    node->next = NULL;
    queue->size++;
  }
  return node;
}

dq_node* dq_add_before(dq_node* ref_node, dq_node* node) {
  dequeue* queue = ref_node->queue;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->head == ref_node) {
    return dq_add_head(queue, node);
  } else {
    dq_link(ref_node->prev, node);
    dq_link(node, ref_node);
    node->queue = queue;
    queue->size++;
    return node;
  }
}

dq_node* dq_add_after(dq_node* ref_node, dq_node* node) {
  dequeue* queue = ref_node->queue;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->tail == ref_node) {
    return dq_add_tail(queue, node);
  } else {
    dq_link(node, ref_node->next);
    dq_link(ref_node, node);
    node->queue = queue;
    queue->size++;
    return node;
  }
}

dq_node* dq_remove_head(dequeue* queue) {
  assert(queue->size != 0, "Cannot remove head from an empty queue");
  if (queue->size == 1) {
    dq_node* n = queue->head;
    dq_clear(queue);
    return n;
  } else {
    dq_node* n = queue->head;
    queue->head = queue->head->next;
    queue->head->prev = NULL;
    queue->size--;
    return n;
  }
}

dq_node* dq_remove_tail(dequeue* queue) {
  assert(queue->size != 0, "Cannot remove tail from an empty queue");
  if (queue->size == 1) {
    dq_node* n = queue->head;
    dq_clear(queue);
    return n;
  } else {
    dq_node* n = queue->tail;
    queue->tail = queue->tail->prev;
    queue->tail->next = NULL;
    queue->size--;
    return n;
  }
}

dq_node* dq_remove(dq_node* node) {
  dequeue* queue = node->queue;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->head == node) {
    return dq_remove_head(queue);
  } else if (queue->tail == node) {
    return dq_remove_tail(queue);
  } else {
    dq_link(node->prev, node->next);
    queue->size--;
    return node;
  }
}

void dq_clear(dequeue* queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
}

void dq_iterate(dequeue* queue, bool (* process)(dq_node*)) {
  for (dq_node* n = queue->head; n != NULL; n = n->next) {
    if (!process(n)) {
      return;
    }
  }
}

char* dq_check(dequeue* queue) {
  if (queue->size == 0) {
    if (queue->head != NULL) return "The head of empty list must be NULL";
    if (queue->tail != NULL) return "The tail of empty list must be NULL";
  } else {
    if (queue->head->prev != NULL) return "The head of list must not have prev";
    if (queue->tail->next != NULL) return "The tail of list must not have next";
    for (dq_node* n = queue->head; n != NULL; n = n->next) {
      if (n->queue != queue) {
        return "The node does not belong to the queue";
      }
    }
  }
  return NULL;
}

SUITE(dequeue)

  TEST(dq_add_head_1)
    dq_node node1 = {0};
    dequeue dq = {0};
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
    dq_add_head(&dq, &node1);
    ASSERT_EQUAL_INT(dq.size, 1);
    ASSERT_EQUAL_PTR(dq.head, &node1);
    ASSERT_EQUAL_PTR(dq.tail, &node1);
    ASSERT_EQUAL_PTR(node1.queue, &dq);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_head_2)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
    dq_add_head(&dq, &node1);
    dq_add_head(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq.head, &node2);
    ASSERT_EQUAL_PTR(dq.tail, &node1);
    ASSERT_EQUAL_PTR(node2.next, &node1);
    ASSERT_EQUAL_PTR(node1.prev, &node2);
    ASSERT_EQUAL_PTR(node1.queue, &dq);
    ASSERT_EQUAL_PTR(node2.queue, &dq);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_tail_1)
    dq_node node1 = {0};
    dequeue dq = {0};
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
    dq_add_tail(&dq, &node1);
    ASSERT_EQUAL_INT(dq.size, 1);
    ASSERT_EQUAL_PTR(dq.head, &node1);
    ASSERT_EQUAL_PTR(dq.tail, &node1);
    ASSERT_EQUAL_PTR(node1.queue, &dq);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_tail_2)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
    dq_add_tail(&dq, &node1);
    dq_add_tail(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq.head, &node1);
    ASSERT_EQUAL_PTR(dq.tail, &node2);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.queue, &dq);
    ASSERT_EQUAL_PTR(node2.queue, &dq);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_remove_head)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    dq_add_tail(&dq, &node1);
    dq_add_tail(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq_remove_head(&dq), &node1);
    ASSERT_EQUAL_INT(dq.size, 1);
    ASSERT_EQUAL_PTR(dq.head, &node2);
    ASSERT_EQUAL_PTR(dq_remove_head(&dq), &node2);
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
//    ASSERT_ASSERTS(dq_remove_head(&dq), "Cannot remove head from an empty queue");
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_remove_tail)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    dq_add_tail(&dq, &node1);
    dq_add_tail(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq_remove_tail(&dq), &node2);
    ASSERT_EQUAL_INT(dq.size, 1);
    ASSERT_EQUAL_PTR(dq.tail, &node1);
    ASSERT_EQUAL_PTR(dq_remove_tail(&dq), &node1);
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq.tail);
//    ASSERT_ASSERTS(dq_remove_tail(&dq), "Cannot remove tail from an empty queue");
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_remove)
    dq_node node1 = {0}, node2 = {0}, node3 = {0}, node4 = {0};
    dequeue dq = {0};
    dq_add_tail(&dq, &node1);
    dq_add_tail(&dq, &node2);
    dq_add_tail(&dq, &node3);
    dq_add_tail(&dq, &node4);
    ASSERT_EQUAL_INT(dq.size, 4);
    ASSERT_EQUAL_PTR(dq_remove(&node2), &node2);
    ASSERT_EQUAL_INT(dq.size, 3);
    ASSERT_EQUAL_PTR(node1.next, &node3);
    ASSERT_EQUAL_PTR(node3.prev, &node1);
    ASSERT_EQUAL_PTR(dq_remove(&node1), &node1);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq_remove(&node4), &node4);
    ASSERT_EQUAL_INT(dq.size, 1);
    ASSERT_EQUAL_PTR(dq.head, &node3);
    ASSERT_EQUAL_PTR(dq.tail, &node3);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_after_tail)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    dq_add_head(&dq, &node1);
    ASSERT_EQUAL_INT(dq.size, 1);
    dq_add_after(&node1, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq.tail, &node2);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_after_mid)
    dq_node node1 = {0}, node2 = {0}, node3 = {0};
    dequeue dq = {0};
    dq_add_head(&dq, &node1);
    dq_add_tail(&dq, &node3);
    ASSERT_EQUAL_INT(dq.size, 2);
    dq_add_after(&node1, &node2);
    ASSERT_EQUAL_INT(dq.size, 3);
    ASSERT_EQUAL_PTR(node2.next, &node3);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node3.prev, &node2);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_before_head)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    dq_add_head(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 1);
    dq_add_before(&node2, &node1);
    ASSERT_EQUAL_INT(dq.size, 2);
    ASSERT_EQUAL_PTR(dq.tail, &node2);
    ASSERT_EQUAL_PTR(dq.head, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_add_before_mid)
    dq_node node1 = {0}, node2 = {0}, node3 = {0};
    dequeue dq = {0};
    dq_add_head(&dq, &node1);
    dq_add_tail(&dq, &node3);
    ASSERT_EQUAL_INT(dq.size, 2);
    dq_add_before(&node3, &node2);
    ASSERT_EQUAL_INT(dq.size, 3);
    ASSERT_EQUAL_PTR(node2.next, &node3);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node3.prev, &node2);
    ASSERT_NULL(dq_check(&dq));
    ENDT

  TEST(dq_clear)
    dq_node node1 = {0}, node2 = {0};
    dequeue dq = {0};
    dq_add_tail(&dq, &node1);
    dq_add_tail(&dq, &node2);
    ASSERT_EQUAL_INT(dq.size, 2);
    dq_clear(&dq);
    ASSERT_EQUAL_INT(dq.size, 0);
    ASSERT_NULL(dq.tail);
    ASSERT_NULL(dq.head);
    ASSERT_NULL(dq_check(&dq));
    ENDT

ENDS
