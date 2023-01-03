#ifndef KERNEL_ARCH_X86_64_DLL_H_
#define KERNEL_ARCH_X86_64_DLL_H_

#include <stdint.h>
#include <kernel/error.h>
#include "test.h"

typedef struct dll_list dll_list;
typedef struct dll_node dll_node;

struct dll_list {
  dll_node* head;
  dll_node* tail;
  uint32_t size;
};

struct dll_node {
  dll_node* prev;
  dll_node* next;
  dll_list* list;
};

dll_node* dll_add_head(dll_list* queue, dll_node* node);
dll_node* dll_add_tail(dll_list* queue, dll_node* node);
dll_node* dll_add_before(dll_node* ref_node, dll_node* node);
dll_node* dll_add_after(dll_node* ref_node, dll_node* node);
dll_node* dll_remove_head(dll_list* queue);
dll_node* dll_remove_tail(dll_list* queue);
dll_node* dll_remove(dll_node* node);
void dll_clear(dll_list* queue);

SUITEDECL(dll);

#endif //KERNEL_ARCH_X86_64_DLL_H_
