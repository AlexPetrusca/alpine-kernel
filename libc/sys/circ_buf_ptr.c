#include <sys/circ_buf_ptr.h>

void circ_buf_ptr_init(circ_buf_ptr_t ptr, void* ptr_raw, void* buf, size_t buf_capacity, size_t granularity) {
  ptr->ptr = (char*) ptr_raw;
  ptr->buf = (char*) buf;
  ptr->buf_capacity = buf_capacity;
  ptr->granularity = granularity;
}

void circ_buf_ptr_init2(circ_buf_ptr_t ptr, void* ptr_raw, circ_buf* cb) {
  circ_buf_ptr_init(ptr, ptr_raw, cb->buf, cb->capacity, cb->granularity);
}

void* circ_buf_ptr_increment(circ_buf_ptr_t ptr) {
  ptr->ptr += ptr->granularity;
  if (ptr->ptr == ptr->buf + ptr->buf_capacity * ptr->granularity) {
    ptr->ptr = ptr->buf;
  }
  return ptr->ptr;
}

void* circ_buf_ptr_decrement(circ_buf_ptr_t ptr) {
  if (ptr->ptr == ptr->buf) {
    ptr->ptr = ptr->buf + ptr->buf_capacity * ptr->granularity;
  }
  ptr->ptr -= ptr->granularity;
  return ptr->ptr;
}

void* circ_buf_ptr_add(circ_buf_ptr_t ptr, size_t n) {
  for (size_t i = 0; i < n; i++) {
    circ_buf_ptr_increment(ptr);
  }
  return ptr->ptr;
}

void* circ_buf_ptr_subtract(circ_buf_ptr_t ptr, size_t n) {
  for (size_t i = 0; i < n; i++) {
    circ_buf_ptr_decrement(ptr);
  }
  return ptr->ptr;
}

void* circ_buf_ptr_getoffset(circ_buf_ptr_t ptr, int n) {
  struct circ_buf_ptr offset_ptr;
  circ_buf_ptr_init(&offset_ptr, ptr->ptr, ptr->buf, ptr->buf_capacity, ptr->granularity);
  if (n > 0) {
    return circ_buf_ptr_add(&offset_ptr, n);
  } else {
    return circ_buf_ptr_subtract(&offset_ptr, -n);
  }
}