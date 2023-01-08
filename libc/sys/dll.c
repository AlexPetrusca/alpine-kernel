#include <stddef.h>
#include <assert.h>

#include <sys/dll.h>

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
