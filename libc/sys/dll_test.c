#include <kernel/test/test.h>
#include <sys/dll.h>

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

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
  END_TEST

END_SUITE