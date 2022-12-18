#ifndef _SYS_CIRC_BUF_H
#define _SYS_CIRC_BUF_H

#include <stdbool.h>
#include <stddef.h>

#include <sys/cdefs.h>

typedef struct circ_buf {
	char * buf;
	size_t head;
	size_t tail;
    size_t size;
    size_t capacity;
    size_t granularity;
} * circ_buf_t;

__BEGIN_DECLS

void circ_buf_init(circ_buf_t cb, void * buf, size_t capacity, size_t granularity);

void circ_buf_enqueue(circ_buf_t cb, void * val);
void * circ_buf_dequeue(circ_buf_t cb);
void circ_buf_push(circ_buf_t cb, void * val);
void * circ_buf_pop(circ_buf_t cb);
void * circ_buf_peek(circ_buf_t cb);
void * circ_buf_peekn(circ_buf_t cb, size_t n);

void circ_buf_enqueue_head(circ_buf_t cb, void * val);
void circ_buf_enqueue_tail(circ_buf_t cb, void * val);
void * circ_buf_dequeue_head(circ_buf_t cb);
void * circ_buf_dequeue_tail(circ_buf_t cb);
void * circ_buf_peek_head(circ_buf_t cb);
void * circ_buf_peekn_head(circ_buf_t cb, size_t n);
void * circ_buf_peek_tail(circ_buf_t cb);
void * circ_buf_peekn_tail(circ_buf_t cb, size_t n);
bool circ_buf_empty(circ_buf_t cb);
bool circ_buf_full(circ_buf_t cb);

__END_DECLS

#endif
