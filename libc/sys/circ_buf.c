#include <stddef.h>
#include <stdlib.h>

#include <sys/circ_buf.h>

circ_buf* circ_buf_new(size_t capacity, size_t granularity) {
  circ_buf* cb = malloc(sizeof(circ_buf));
  char* buf = malloc(capacity * granularity);
  circ_buf_init(cb, buf, capacity, granularity);
  return cb;
}

void circ_buf_free(circ_buf* cb) {
  free(cb->buf);
  free(cb);
}

void circ_buf_init(circ_buf* cb, void* buf, size_t capacity, size_t granularity) {
  cb->buf = (char*) buf;
  cb->head = 0;
  cb->tail = 0;
  cb->size = 0;
  cb->capacity = capacity;
  cb->granularity = granularity;
}

static inline size_t circ_buf_granular_index(circ_buf* cb, size_t index) {
  return index * cb->granularity;
}

static inline void circ_buf_increment_head(circ_buf* cb) {
  cb->head = (cb->head + 1) % cb->capacity;
}

static inline void circ_buf_decrement_head(circ_buf* cb) {
  cb->head = (cb->head + cb->capacity - 1) % cb->capacity;
}

static inline void circ_buf_increment_tail(circ_buf* cb) {
  cb->tail = (cb->tail + 1) % cb->capacity;
}

static inline void circ_buf_decrement_tail(circ_buf* cb) {
  cb->tail = (cb->tail + cb->capacity - 1) % cb->capacity;
}

void circ_buf_enqueue_head(circ_buf* cb, void* val) {
  circ_buf_decrement_head(cb);
  for (size_t i = 0; i < cb->granularity; i++) {
    cb->buf[circ_buf_granular_index(cb, cb->head) + i] = ((char*) val)[i];
  }
  if (cb->size < cb->capacity) {
    cb->size++;
  } else {
    circ_buf_decrement_tail(cb);
  }
}

void circ_buf_enqueue_tail(circ_buf* cb, void* val) {
  for (size_t i = 0; i < cb->granularity; i++) {
    cb->buf[circ_buf_granular_index(cb, cb->tail) + i] = ((char*) val)[i];
  }
  circ_buf_increment_tail(cb);
  if (cb->size < cb->capacity) {
    cb->size++;
  } else {
    circ_buf_increment_head(cb);
  }
}

void* circ_buf_dequeue_head(circ_buf* cb) {
  if (cb->size > 0) {
    void* val = cb->buf + circ_buf_granular_index(cb, cb->head);
    circ_buf_increment_head(cb);
    cb->size--;
    return val;
  }
  // todo: set errno
  return NULL;
}

void* circ_buf_dequeue_tail(circ_buf* cb) {
  if (cb->size > 0) {
    circ_buf_decrement_tail(cb);
    void* val = cb->buf + circ_buf_granular_index(cb, cb->tail);
    cb->size--;
    return val;
  }
  // todo: set errno
  return NULL;
}

void* circ_buf_peek_head(circ_buf* cb) {
  return cb->buf + circ_buf_granular_index(cb, cb->head);
}

void* circ_buf_peekn_head(circ_buf* cb, size_t n) {
  size_t offset = (cb->head + n) % cb->capacity;
  return cb->buf + circ_buf_granular_index(cb, offset);
}

void* circ_buf_peek_tail(circ_buf* cb) {
  return cb->buf + circ_buf_granular_index(cb, cb->tail - 1);
}

void* circ_buf_peekn_tail(circ_buf* cb, size_t n) {
  size_t offset = ((cb->tail - n - 1) % cb->capacity + cb->capacity) % cb->capacity;
  return cb->buf + circ_buf_granular_index(cb, offset);
}

void circ_buf_enqueue(circ_buf* cb, void* val) {
  circ_buf_enqueue_tail(cb, val);
}

void* circ_buf_dequeue(circ_buf* cb) {
  return circ_buf_dequeue_head(cb);
}

void* circ_buf_peek(circ_buf* cb) {
  return circ_buf_peek_head(cb);
}

void* circ_buf_peekn(circ_buf* cb, size_t n) {
  return circ_buf_peekn_head(cb, n);
}

void circ_buf_push(circ_buf* cb, void* val) {
  circ_buf_enqueue_head(cb, val);
}

void* circ_buf_pop(circ_buf* cb) {
  return circ_buf_dequeue_head(cb);
}

bool circ_buf_empty(circ_buf* cb) {
  return cb->size == 0;
}

bool circ_buf_full(circ_buf* cb) {
  return cb->size == cb->capacity;
}
