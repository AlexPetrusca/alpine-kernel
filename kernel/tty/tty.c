#include <stddef.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <sys/circ_buf_ptr.h>
#include <kernel/boot/mb2_type.h>
#include <kernel/mem/mem.h>
#include <kernel/tty/vbe_ttyd.h>
#include <kernel/tty/vga_ttyd.h>
#include <kernel/tty/tty.h>
#include <stdio.h>
#include <assert.h>

#define TTY_BUFFER_SIZE 65535
#define TTY_LINE_LIMIT 1000
#define TAB_LENGTH 8

char tty_buf[TTY_BUFFER_SIZE];
struct circ_buf_ptr tty_buf_wptr;

char* tty_line_buf[TTY_LINE_LIMIT];
circ_buf tty_circ_line_buf;
struct circ_buf_ptr tty_line_buf_rptr;

tty_device* ttyd;
uint32_t tty_line;
uint32_t tty_cursorx, tty_cursory;
uint32_t tty_width, tty_height;

void tty_writestring_raw(const char* data);
void tty_write_raw(const char* data, size_t size);
void tty_putchar_raw(char c);

static inline char* tty_cursorxy_to_buf_wptr(uint32_t x, uint32_t y) {
  struct circ_buf_ptr line_ptr;
  char* line = *(char**) circ_buf_ptr_getoffset(&tty_line_buf_rptr, y);
  circ_buf_ptr_init(&line_ptr, line, tty_buf, TTY_BUFFER_SIZE, sizeof(char));
  return circ_buf_ptr_add(&line_ptr, x);
}

static inline char* tty_cursor_to_buf_wptr(uint32_t pos) {
  return tty_cursorxy_to_buf_wptr(pos % tty_width, pos / tty_width);
}

static inline char* tty_buf_dequeue_line() {
  return *(char**) circ_buf_dequeue(&tty_circ_line_buf);
}

static inline char* tty_buf_read_line(size_t n) {
  return *(char**) circ_buf_peekn(&tty_circ_line_buf, n);
}

void tty_buf_write_null_terminator() {
  if (tty_circ_line_buf.size > 1 && tty_buf_wptr.ptr == tty_buf_read_line(0)) {
    tty_buf_dequeue_line();
  }
  *tty_buf_wptr.ptr = '\0';
}

void tty_buf_enqueue_line(char* line) {
  circ_buf_enqueue(&tty_circ_line_buf, &line);
  tty_buf_write_null_terminator();
}

__attribute__ ((warn_unused_result))
bool ttyd_init(mb2_tag_framebuffer* framebuffer_tag) {
  mb2_tag_framebuffer_common fb_info = framebuffer_tag->common;
  if (fb_info.framebuffer_type == MB2_FRAMEBUFFER_TYPE_RGB) {
    size_t fb_size = (fb_info.framebuffer_width * fb_info.framebuffer_height) * sizeof(uint32_t);
    try(mem_map_range_4k(fb_info.framebuffer_addr, fb_info.framebuffer_addr, fb_size), false, "");
    mem_update_range(fb_info.framebuffer_addr, fb_info.framebuffer_addr, fb_size, MEMORY_FRAME_BUFFER);
    *ttyd = vbe_ttyd_init(fb_info.framebuffer_addr, fb_info.framebuffer_width, fb_info.framebuffer_height);
  } else {
    *ttyd = vga_ttyd_init(VGA_TEXT_MODE_PHYS_ADDR);
  }
  return true;
}

void tty_init(mb2_tag_framebuffer* fb_tag) {
  assert(ttyd_init(fb_tag), "");
  tty_width = ttyd->get_width();
  tty_height = ttyd->get_height();
  tty_setcolor(COLOR_LIGHT_GREEN, COLOR_BLACK);
  tty_clear();
}

uint32_t tty_get_width() {
  return tty_width;
}

uint32_t tty_get_height() {
  return tty_height;
}

void tty_clear() {
  circ_buf_init(&tty_circ_line_buf, tty_line_buf, TTY_LINE_LIMIT, sizeof(char*));
  circ_buf_ptr_init(&tty_buf_wptr, tty_buf, tty_buf, TTY_BUFFER_SIZE, sizeof(char));
  circ_buf_ptr_init(&tty_line_buf_rptr, tty_line_buf, tty_line_buf, TTY_LINE_LIMIT, sizeof(char*));
  tty_buf_enqueue_line(tty_buf_wptr.ptr);

  ttyd->clear();
  tty_set_cursor_pos_xy(0, 0);
  tty_line = 0;
}

void tty_delete(size_t n) {
  char* tail_ptr = *(char**) circ_buf_peek_tail(&tty_circ_line_buf);
  for (size_t i = 0; i < n; i++) {
    if (tty_buf_wptr.ptr == tail_ptr) {
      circ_buf_dequeue_tail(&tty_circ_line_buf);
      tail_ptr = *(char**) circ_buf_peek_tail(&tty_circ_line_buf);
    }
    uint32_t pos = tty_get_cursor_pos();
    tty_set_cursor_pos(pos - 1);
    ttyd->put_char(tty_cursorx, tty_cursory, ' ');
    *tty_buf_wptr.ptr = '\0';
  }
}

void tty_setcolor(tty_color fg, tty_color bg) {
  ttyd->set_fg(fg);
  ttyd->set_bg(bg);
}

uint32_t tty_getline() {
  return tty_line;
}

void tty_enable_cursor() {
  ttyd->enable_cursor();
}

void tty_disable_cursor() {
  ttyd->disable_cursor();
}

void tty_set_cursor_pos(uint64_t pos) {
  tty_set_cursor_pos_xy(pos % tty_width, pos / tty_width);
}

void tty_set_cursor_pos_xy(uint32_t x, uint32_t y) {
  tty_cursorx = x;
  tty_cursory = y;
  tty_buf_wptr.ptr = tty_cursorxy_to_buf_wptr(x, y);
  ttyd->set_cursor(x, y);
}

uint32_t tty_get_cursor_pos() {
  return tty_cursory * tty_width + tty_cursorx;
}

void tty_flush_line(size_t y) {
  uint32_t cursor_x_temp = tty_cursorx;
  uint32_t cursor_y_temp = tty_cursory;
  tty_set_cursor_pos_xy(0, y);

  struct circ_buf_ptr line_ptr;
  char* line = *(char**) circ_buf_ptr_getoffset(&tty_line_buf_rptr, y);
  circ_buf_ptr_init(&line_ptr, line, tty_buf, TTY_BUFFER_SIZE, sizeof(char));
  while (*line_ptr.ptr != '\0') {
    tty_putchar_raw(*line_ptr.ptr);
    circ_buf_ptr_increment(&line_ptr);
  }

  tty_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
}

void tty_flush() {
  ttyd->clear();

  uint32_t cursor_x_temp = tty_cursorx;
  uint32_t cursor_y_temp = tty_cursory;
  tty_set_cursor_pos_xy(0, 0);

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, tty_buf, tty_buf, TTY_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < tty_height; i++) {
    line_ptr.ptr = *(char**) circ_buf_ptr_getoffset(&tty_line_buf_rptr, i);
    while (*line_ptr.ptr != '\0') {
      tty_putchar_raw(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
    if (i != tty_height - 1) {
      tty_putchar_raw('\n');
    }
  }

  tty_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
}

void tty_scroll_up_force() {
  tty_line--;
  circ_buf_ptr_decrement(&tty_line_buf_rptr);
  tty_flush();
}

void tty_scroll_down_force() {
  tty_line++;
  circ_buf_ptr_increment(&tty_line_buf_rptr);
  tty_flush();
}

void tty_scroll_up() {
  if ((char**) tty_line_buf_rptr.ptr != tty_line_buf + tty_circ_line_buf.head) {
    tty_scroll_up_force();
  }
}

void tty_scroll_down() {
  if ((char**) circ_buf_ptr_getoffset(&tty_line_buf_rptr, tty_cursory)
    != tty_line_buf + tty_circ_line_buf.tail - 1) {
    tty_scroll_down_force();
  }
}

void tty_buf_write(char c) {
  if (tty_circ_line_buf.size > 1 && tty_buf_wptr.ptr == tty_buf_read_line(0)) {
    tty_buf_dequeue_line();
  }
  if (c == '\n') {
    *tty_buf_wptr.ptr = '\0';
    circ_buf_ptr_increment(&tty_buf_wptr);
    tty_buf_enqueue_line(tty_buf_wptr.ptr);
  } else {
    if (tty_cursorx == tty_width) {
      circ_buf_ptr_increment(&tty_buf_wptr);
      tty_buf_enqueue_line(tty_buf_wptr.ptr);
    }
    *tty_buf_wptr.ptr = c;
    circ_buf_ptr_increment(&tty_buf_wptr);
    tty_buf_write_null_terminator();
  }
}

static inline void tty_new_line() {
  tty_cursorx = 0;
  if (tty_cursory == tty_height - 1) {
    tty_scroll_down_force();
  } else {
    tty_cursory++;
  }
}

void tty_putchar_raw(char c) {
  switch (c) {
    case '\t':
      while ((tty_cursorx + 1) % TAB_LENGTH != 0) {
        tty_cursorx++;
      }
      if (tty_cursorx < tty_width - 1) {
        tty_cursorx++;
      }
      break;
    case '\b':
      tty_cursorx -= (tty_cursorx == 0) ? 0 : 1;
      break;
    case '\n':
      tty_new_line();
      break;
    default:
      if (tty_cursorx == tty_width) {
        tty_new_line();
      }
      ttyd->put_char(tty_cursorx, tty_cursory, c);
      tty_cursorx++;
  }
  tty_set_cursor_pos_xy(tty_cursorx, tty_cursory);
}

void tty_putchar(char c) {
  tty_buf_write(c);
  tty_putchar_raw(c);
}

void tty_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    tty_putchar(data[i]);
  }
}

void tty_writestring(const char* data) {
  tty_write(data, strlen(data));
}

void tty_write_raw(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    tty_putchar_raw(data[i]);
  }
}

void tty_writestring_raw(const char* data) {
  tty_write_raw(data, strlen(data));
}

// manual tests:
// buffer limit: TTY_BUFFER_SIZE = 300 --> a, b, c, d, ... until DEQUEUE (check validity)
// line limit: TTY_LINE_LIMIT = 30 --> a, b, c, d, ... until DEQUEUE (check validity + only scroll by 5)

// test writing multi-line sh_commands
// test recalling multi-line sh_commands from history
// test scrolling after writing a multi-line command (2 lines) and then deleting down to 1 line
// test scrolling after recalling multi-line command (4+ lines) then recalling small command (1 line)