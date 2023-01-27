#ifndef _SYS_CIRC_BUF_H
#define _SYS_CIRC_BUF_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/cdefs.h>

typedef struct {
  char* buf;
  size_t head;
  size_t tail;
  size_t size;
  size_t capacity; // max number of elements
  size_t granularity; // element size in bytes
} circ_buf;

__BEGIN_DECLS

circ_buf* circ_buf_new(size_t capacity, size_t granularity);
void circ_buf_free(circ_buf* cb);
void circ_buf_init(circ_buf* cb, void* buf, size_t capacity, size_t granularity);

void circ_buf_enqueue(circ_buf* cb, void* val);
void* circ_buf_dequeue(circ_buf* cb);
void circ_buf_push(circ_buf* cb, void* val);
void* circ_buf_pop(circ_buf* cb);
void* circ_buf_peek(circ_buf* cb);
void* circ_buf_peekn(circ_buf* cb, size_t n);

void circ_buf_enqueue_head(circ_buf* cb, void* val);
void circ_buf_enqueue_tail(circ_buf* cb, void* val);
void* circ_buf_dequeue_head(circ_buf* cb);
void* circ_buf_dequeue_tail(circ_buf* cb);
void* circ_buf_peek_head(circ_buf* cb);
void* circ_buf_peekn_head(circ_buf* cb, size_t n);
void* circ_buf_peek_tail(circ_buf* cb);
void* circ_buf_peekn_tail(circ_buf* cb, size_t n);
bool circ_buf_empty(circ_buf* cb);
bool circ_buf_full(circ_buf* cb);

__END_DECLS

#endif
