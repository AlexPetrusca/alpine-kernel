#include <sys/dll.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/test/test.h>
#include <assert.h>

static inline void dll_link(dll_node* node1, dll_node* node2) {
  node1->next = node2;
  node2->prev = node1;
}

static inline void dll_add_only(dll_list* queue, dll_node* node) {
  queue->head = node;
  queue->tail = node;
  node->prev = NULL;
  node->next = NULL;
  node->list = queue;
  queue->size = 1;
}

dll_node* dll_add_head(dll_list* queue, dll_node* node) {
  if (queue->size == 0) {
    dll_add_only(queue, node);
  } else {
    dll_link(node, queue->head);
    queue->head = node;
    node->list = queue;
    node->prev = NULL;
    queue->size++;
  }
  return node;
}

dll_node* dll_add_tail(dll_list* queue, dll_node* node) {
  if (queue->size == 0) {
    dll_add_only(queue, node);
  } else {
    dll_link(queue->tail, node);
    queue->tail = node;
    node->list = queue;
    node->next = NULL;
    queue->size++;
  }
  return node;
}

dll_node* dll_add_before(dll_node* ref_node, dll_node* node) {
  dll_list* queue = ref_node->list;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->head == ref_node) {
    return dll_add_head(queue, node);
  } else {
    dll_link(ref_node->prev, node);
    dll_link(node, ref_node);
    node->list = queue;
    queue->size++;
    return node;
  }
}

dll_node* dll_add_after(dll_node* ref_node, dll_node* node) {
  dll_list* queue = ref_node->list;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->tail == ref_node) {
    return dll_add_tail(queue, node);
  } else {
    dll_link(node, ref_node->next);
    dll_link(ref_node, node);
    node->list = queue;
    queue->size++;
    return node;
  }
}

dll_node* dll_remove_head(dll_list* queue) {
  assert(queue->size != 0, "Cannot remove head from an empty queue");
  if (queue->size == 1) {
    dll_node* n = queue->head;
    dll_clear(queue);
    return n;
  } else {
    dll_node* n = queue->head;
    queue->head = queue->head->next;
    queue->head->prev = NULL;
    queue->size--;
    return n;
  }
}

dll_node* dll_remove_tail(dll_list* queue) {
  assert(queue->size != 0, "Cannot remove tail from an empty queue");
  if (queue->size == 1) {
    dll_node* n = queue->head;
    dll_clear(queue);
    return n;
  } else {
    dll_node* n = queue->tail;
    queue->tail = queue->tail->prev;
    queue->tail->next = NULL;
    queue->size--;
    return n;
  }
}

dll_node* dll_remove(dll_node* node) {
  dll_list* queue = node->list;
  assert(queue != NULL, "Node does not belong to a queue.");
  if (queue->head == node) {
    return dll_remove_head(queue);
  } else if (queue->tail == node) {
    return dll_remove_tail(queue);
  } else {
    dll_link(node->prev, node->next);
    queue->size--;
    return node;
  }
}

void dll_clear(dll_list* queue) {
  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
}

char* dll_check(dll_list* queue) {
  if (queue->size == 0) {
    if (queue->head != NULL) return "The head of empty list must be NULL";
    if (queue->tail != NULL) return "The tail of empty list must be NULL";
  } else {
    if (queue->head->prev != NULL) return "The head of list must not have prev";
    if (queue->tail->next != NULL) return "The tail of list must not have next";
    for (dll_node* n = queue->head; n != NULL; n = n->next) {
      if (n->list != queue) {
        return "The node does not belong to the queue";
      }
    }
  }
  return NULL;
}

SUITE(dll)

  TEST(dll_add_head_1)
    dll_node node1 = {0};
    dll_list list = {0};
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    dll_add_head(&list, &node1);
    ASSERT_EQUAL_INT(list.size, 1);
    ASSERT_EQUAL_PTR(list.head, &node1);
    ASSERT_EQUAL_PTR(list.tail, &node1);
    ASSERT_EQUAL_PTR(node1.list, &list);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_head_2)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    dll_add_head(&list, &node1);
    dll_add_head(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(list.head, &node2);
    ASSERT_EQUAL_PTR(list.tail, &node1);
    ASSERT_EQUAL_PTR(node2.next, &node1);
    ASSERT_EQUAL_PTR(node1.prev, &node2);
    ASSERT_EQUAL_PTR(node1.list, &list);
    ASSERT_EQUAL_PTR(node2.list, &list);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_tail_1)
    dll_node node1 = {0};
    dll_list list = {0};
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    dll_add_tail(&list, &node1);
    ASSERT_EQUAL_INT(list.size, 1);
    ASSERT_EQUAL_PTR(list.head, &node1);
    ASSERT_EQUAL_PTR(list.tail, &node1);
    ASSERT_EQUAL_PTR(node1.list, &list);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_tail_2)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    dll_add_tail(&list, &node1);
    dll_add_tail(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(list.head, &node1);
    ASSERT_EQUAL_PTR(list.tail, &node2);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.list, &list);
    ASSERT_EQUAL_PTR(node2.list, &list);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_remove_head)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    dll_add_tail(&list, &node1);
    dll_add_tail(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(dll_remove_head(&list), &node1);
    ASSERT_EQUAL_INT(list.size, 1);
    ASSERT_EQUAL_PTR(list.head, &node2);
    ASSERT_EQUAL_PTR(dll_remove_head(&list), &node2);
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_remove_tail)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    dll_add_tail(&list, &node1);
    dll_add_tail(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(dll_remove_tail(&list), &node2);
    ASSERT_EQUAL_INT(list.size, 1);
    ASSERT_EQUAL_PTR(list.tail, &node1);
    ASSERT_EQUAL_PTR(dll_remove_tail(&list), &node1);
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.head);
    ASSERT_NULL(list.tail);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_remove)
    dll_node node1 = {0}, node2 = {0}, node3 = {0}, node4 = {0};
    dll_list list = {0};
    dll_add_tail(&list, &node1);
    dll_add_tail(&list, &node2);
    dll_add_tail(&list, &node3);
    dll_add_tail(&list, &node4);
    ASSERT_EQUAL_INT(list.size, 4);
    ASSERT_EQUAL_PTR(dll_remove(&node2), &node2);
    ASSERT_EQUAL_INT(list.size, 3);
    ASSERT_EQUAL_PTR(node1.next, &node3);
    ASSERT_EQUAL_PTR(node3.prev, &node1);
    ASSERT_EQUAL_PTR(dll_remove(&node1), &node1);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(dll_remove(&node4), &node4);
    ASSERT_EQUAL_INT(list.size, 1);
    ASSERT_EQUAL_PTR(list.head, &node3);
    ASSERT_EQUAL_PTR(list.tail, &node3);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_after_tail)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    dll_add_head(&list, &node1);
    ASSERT_EQUAL_INT(list.size, 1);
    dll_add_after(&node1, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(list.tail, &node2);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_after_mid)
    dll_node node1 = {0}, node2 = {0}, node3 = {0};
    dll_list list = {0};
    dll_add_head(&list, &node1);
    dll_add_tail(&list, &node3);
    ASSERT_EQUAL_INT(list.size, 2);
    dll_add_after(&node1, &node2);
    ASSERT_EQUAL_INT(list.size, 3);
    ASSERT_EQUAL_PTR(node2.next, &node3);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node3.prev, &node2);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_before_head)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    dll_add_head(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 1);
    dll_add_before(&node2, &node1);
    ASSERT_EQUAL_INT(list.size, 2);
    ASSERT_EQUAL_PTR(list.tail, &node2);
    ASSERT_EQUAL_PTR(list.head, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_add_before_mid)
    dll_node node1 = {0}, node2 = {0}, node3 = {0};
    dll_list list = {0};
    dll_add_head(&list, &node1);
    dll_add_tail(&list, &node3);
    ASSERT_EQUAL_INT(list.size, 2);
    dll_add_before(&node3, &node2);
    ASSERT_EQUAL_INT(list.size, 3);
    ASSERT_EQUAL_PTR(node2.next, &node3);
    ASSERT_EQUAL_PTR(node2.prev, &node1);
    ASSERT_EQUAL_PTR(node1.next, &node2);
    ASSERT_EQUAL_PTR(node3.prev, &node2);
    ASSERT_NULL(dll_check(&list));
    ENDT

  TEST(dll_clear)
    dll_node node1 = {0}, node2 = {0};
    dll_list list = {0};
    dll_add_tail(&list, &node1);
    dll_add_tail(&list, &node2);
    ASSERT_EQUAL_INT(list.size, 2);
    dll_clear(&list);
    ASSERT_EQUAL_INT(list.size, 0);
    ASSERT_NULL(list.tail);
    ASSERT_NULL(list.head);
    ASSERT_NULL(dll_check(&list));
    ENDT

ENDS
