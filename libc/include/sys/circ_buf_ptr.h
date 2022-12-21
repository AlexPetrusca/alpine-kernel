#ifndef _SYS_CIRC_BUF_PTR_H
#define _SYS_CIRC_BUF_PTR_H

#include <stddef.h>

#include <sys/cdefs.h>
#include <sys/circ_buf.h>

typedef struct circ_buf_ptr {
  char* ptr;
  char* buf;
  size_t buf_capacity;
  size_t granularity;
} * circ_buf_ptr_t;

__BEGIN_DECLS

void circ_buf_ptr_init(circ_buf_ptr_t ptr, void* ptr_raw, void* buf, size_t buf_capacity, size_t granularity);
void circ_buf_ptr_init2(circ_buf_ptr_t ptr, void* ptr_raw, circ_buf_t cb);

void* circ_buf_ptr_increment(circ_buf_ptr_t ptr);
void* circ_buf_ptr_decrement(circ_buf_ptr_t ptr);
void* circ_buf_ptr_add(circ_buf_ptr_t ptr, size_t n);
void* circ_buf_ptr_subtract(circ_buf_ptr_t ptr, size_t n);
void* circ_buf_ptr_getoffset(circ_buf_ptr_t ptr, int n);

__END_DECLS

#endif