#include <stddef.h>
#include <string.h>

#include <kernel/mem.h>
#include <kernel/heap.h>

#define NUM_SIZES 32
#define ALIGN 4
#define MIN_SIZE sizeof(dlist)
#define HEADER_SIZE OFFSETOF(heap_chunk, data)

#define CONTAINER(C, l, v) ((C*)(((char*)v) - (intptr_t)&(((C*)0)->l)))
#define OFFSETOF(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)

#define DLIST_INIT(v, l) dlist_init(&v->l)

#define DLIST_REMOVE_FROM(h, d, l)             \
{                                             \
  typeof(**h) **h_ = h, *d_ = d;              \
  dlist *head = &(*h_)->l;                    \
  dlist_remove_from(&head, &d_->l);           \
  if (head == NULL) {                         \
      *h_ = NULL;                             \
  } else {                                    \
      *h_ = CONTAINER(typeof(**h), l, head);  \
  }                                           \
}

#define DLIST_PUSH(h, v, l)                    \
{                                             \
  typeof(*v) **h_ = h, *v_ = v;               \
  dlist *head = &(*h_)->l;                    \
  if (*h_ == NULL) head = NULL;               \
  dlist_push(&head, &v_->l);                  \
  *h_ = CONTAINER(typeof(*v), l, head);       \
}

#define DLIST_POP(h, l)                        \
({                                            \
  typeof(**h) **h_ = h;                       \
  dlist *head = &(*h_)->l;                    \
  dlist *res = dlist_pop(&head);              \
  if (head == NULL) {                         \
      *h_ = NULL;                             \
  } else {                                    \
      *h_ = CONTAINER(typeof(**h), l, head);  \
  }                                           \
  CONTAINER(typeof(**h), l, res);             \
})

typedef struct dlist dlist;
struct dlist {
  dlist* next;
  dlist* prev;
};

typedef struct {
  dlist all;
  int used;
  union {
    char data[0];
    dlist free;
  };
} heap_chunk;

heap_chunk* free_chunk[NUM_SIZES] = {NULL};
size_t mem_free = 0;
size_t mem_used = 0;
size_t mem_meta = 0;
heap_chunk* head = NULL;
heap_chunk* tail = NULL;


// initialize a one element dlist
static inline void dlist_init(dlist* dlist) {
  dlist->next = dlist;
  dlist->prev = dlist;
}

// insert d2 after d1
static inline void dlist_insert_after(dlist* d1, dlist* d2) {
  dlist* n1 = d1->next;
  dlist* e2 = d2->prev;

  d1->next = d2;
  d2->prev = d1;
  e2->next = n1;
  n1->prev = e2;
}

// insert d2 before d1
static inline void dlist_insert_before(dlist* d1, dlist* d2) {
  dlist* e1 = d1->prev;
  dlist* e2 = d2->prev;

  e1->next = d2;
  d2->prev = e1;
  e2->next = d1;
  d1->prev = e2;
}

// remove d from the list
static inline void dlist_remove(dlist* d) {
  d->prev->next = d->next;
  d->next->prev = d->prev;
  d->next = d;
  d->prev = d;
}

// push d2 to the front of the d1p list
static inline void dlist_push(dlist** d1p, dlist* d2) {
  if (*d1p != NULL) {
    dlist_insert_before(*d1p, d2);
  }
  *d1p = d2;
}

// pop the front of the dp list
static inline dlist* dlist_pop(dlist** dp) {
  dlist* d1 = *dp;
  dlist* d2 = d1->next;
  dlist_remove(d1);
  if (d1 == d2) {
    *dp = NULL;
  } else {
    *dp = d2;
  }
  return d1;
}

// remove d2 from the list, advancing d1p if needed
static inline void dlist_remove_from(dlist** d1p, dlist* d2) {
  if (*d1p == d2) {
    dlist_pop(d1p);
  } else {
    dlist_remove(d2);
  }
}

static void heap_chunk_init(heap_chunk* chunk) {
  DLIST_INIT(chunk, all);
  chunk->used = 0;
  DLIST_INIT(chunk, free);
}

static size_t heap_chunk_size(const heap_chunk* chunk) {
  char* end = (char*) (chunk->all.next);
  char* start = (char*) (&chunk->all);
  return (end - start) - HEADER_SIZE;
}

static int heap_chunk_slot(size_t size) {
  int n = -1;
  while (size > 0) {
    ++n;
    size /= 2;
  }
  return n;
}

void mrvn_heap_init(void* mem, size_t size) {
  char* mem_start = (char*) (((intptr_t) mem + ALIGN - 1) & (~(ALIGN - 1)));
  char* mem_end = (char*) (((intptr_t) mem + size) & (~(ALIGN - 1)));
  head = (heap_chunk*) mem_start;
  heap_chunk* first = head + 1;
  tail = ((heap_chunk*) mem_end) - 1;
  heap_chunk_init(head);
  heap_chunk_init(first);
  heap_chunk_init(tail);
  dlist_insert_after(&head->all, &first->all);
  dlist_insert_after(&first->all, &tail->all);
  // mark head/tail as used, so they never get merged
  head->used = 1;
  tail->used = 1;

  size_t len = heap_chunk_size(first);
  int n = heap_chunk_slot(len);
  DLIST_PUSH(&free_chunk[n], first, free);
  mem_free = len - HEADER_SIZE;
  mem_meta = sizeof(heap_chunk) * 2 + HEADER_SIZE;
}

// todo: adjust the size of the heap as required
//  - https://linux.die.net/man/3/malloc (Notes section)
//  - https://linux.die.net/man/2/sbrk
//  - https://linux.die.net/man/2/mmap
void* kmalloc(size_t size) {
  size = (size + ALIGN - 1) & (~(ALIGN - 1));
  if (size < MIN_SIZE) {
    size = MIN_SIZE;
  }

  int n = heap_chunk_slot(size - 1) + 1;
  if (n >= NUM_SIZES) {
    return NULL;
  }

  while (!free_chunk[n]) {
    ++n;
    if (n >= NUM_SIZES) {
      return NULL;
    }
  }

  heap_chunk* chunk = DLIST_POP(&free_chunk[n], free);
  size_t size2 = heap_chunk_size(chunk);
  size_t len = 0;

  if (size + sizeof(heap_chunk) <= size2) {
    heap_chunk* chunk2 = (heap_chunk*) (((char*) chunk) + HEADER_SIZE + size);
    heap_chunk_init(chunk2);
    dlist_insert_after(&chunk->all, &chunk2->all);
    len = heap_chunk_size(chunk2);
    int slot = heap_chunk_slot(len);
    DLIST_PUSH(&free_chunk[slot], chunk2, free);
    mem_meta += HEADER_SIZE;
    mem_free += len - HEADER_SIZE;
  }

  chunk->used = 1;
  mem_free -= size2;
  mem_used += size2 - len - HEADER_SIZE;
  return chunk->data;
}

static void heap_remove_free(heap_chunk* chunk) {
  size_t len = heap_chunk_size(chunk);
  int n = heap_chunk_slot(len);
DLIST_REMOVE_FROM(&free_chunk[n], chunk, free);
mem_free -= len - HEADER_SIZE;
}

static void heap_push_free(heap_chunk* chunk) {
  size_t len = heap_chunk_size(chunk);
  int n = heap_chunk_slot(len);
  DLIST_PUSH(&free_chunk[n], chunk, free);
  mem_free += len - HEADER_SIZE;
}

void kfree(void* ptr) {
  heap_chunk* chunk = (heap_chunk*) ((char*) ptr - HEADER_SIZE);
  heap_chunk* next = CONTAINER(heap_chunk, all, chunk->all.next);
  heap_chunk* prev = CONTAINER(heap_chunk, all, chunk->all.prev);

  mem_used -= heap_chunk_size(chunk);

  if (next->used == 0) {
    // merge in next
    heap_remove_free(next);
    dlist_remove(&next->all);
    mem_meta -= HEADER_SIZE;
    mem_free += HEADER_SIZE;
  }
  if (prev->used == 0) {
    // merge to prev
    heap_remove_free(prev);
    dlist_remove(&chunk->all);
    heap_push_free(prev);
    mem_meta -= HEADER_SIZE;
    mem_free += HEADER_SIZE;
  } else {
    // make chunk as free
    chunk->used = 0;
    DLIST_INIT(chunk, free);
    heap_push_free(chunk);
  }
}

// todo: improve performance
//  - https://stackoverflow.com/questions/1538420/difference-between-malloc-and-calloc
//  - https://man7.org/linux/man-pages/man2/mmap.2.html
void* kcalloc(size_t nmemb, size_t size) {
  size_t alloc_size = nmemb * size;
  void* new = kmalloc(alloc_size);
  if (new) {
    memset(new, 0, alloc_size);
    return new;
  }
  return NULL;
}

// https://codereview.stackexchange.com/questions/151019/implementing-realloc-in-c
void* krealloc(void* ptr, size_t size) {
  if (size == 0) {
    kfree(ptr);
    return NULL;
  } else if (!ptr) {
    return kmalloc(size);
  } else {
    heap_chunk* chunk = (heap_chunk*) ((char*) ptr - HEADER_SIZE);
    size_t old_size = heap_chunk_size(chunk);
    if (size <= old_size) {
      return ptr;
    } else {
      void* new_ptr = kmalloc(size);
      if (new_ptr) {
        memcpy(new_ptr, ptr, old_size);
        kfree(ptr);
      }
      return new_ptr;
    }
  }
}

void heap_init(uint64_t addr, size_t size) {
  mrvn_heap_init((char*) addr, size);
}