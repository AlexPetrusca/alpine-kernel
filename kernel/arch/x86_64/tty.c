#include <stddef.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <sys/circ_buf_ptr.h>
#include <kernel/mb2_type.h>
#include <kernel/mb2_info.h>
#include <kernel/mem.h>
#include <kernel/vbe_ttyd.h>
#include <kernel/vga_ttyd.h>
#include <kernel/tty.h>

#define TERMINAL_BUFFER_SIZE 65535
#define TERMINAL_LINE_LIMIT 1000
#define TAB_LENGTH 8

char terminal_buf[TERMINAL_BUFFER_SIZE];
struct circ_buf_ptr terminal_buf_wptr;

char* terminal_line_buf[TERMINAL_LINE_LIMIT];
struct circ_buf terminal_circ_line_buf;
struct circ_buf_ptr terminal_line_buf_rptr;

tty_device* device;
uint16_t terminal_line;
uint8_t cursor_x, cursor_y;
uint32_t width, height;

void terminal_writestring_raw(const char* data);
void terminal_write_raw(const char* data, size_t size);
void terminal_putchar_raw(char c);

static inline char* terminal_cursorxy_to_buf_wptr(uint8_t x, uint8_t y) {
  struct circ_buf_ptr line_ptr;
  char* line = *((char**) circ_buf_ptr_getoffset(&terminal_line_buf_rptr, y));
  circ_buf_ptr_init(&line_ptr, line, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  return circ_buf_ptr_add(&line_ptr, x);
}

static inline char* terminal_cursor_to_buf_wptr(uint16_t pos) {
  return terminal_cursorxy_to_buf_wptr(pos % width, pos / width);
}

static inline char* terminal_buf_dequeue_line() {
  return *((char**) circ_buf_dequeue(&terminal_circ_line_buf));
}

static inline char* terminal_buf_read_line(size_t n) {
  return *((char**) circ_buf_peekn(&terminal_circ_line_buf, n));
}

void terminal_buf_write_null_terminator() {
  if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr.ptr == terminal_buf_read_line(0)) {
    terminal_buf_dequeue_line();
  }
  *terminal_buf_wptr.ptr = '\0';
}

void terminal_buf_enqueue_line(char* line) {
  circ_buf_enqueue(&terminal_circ_line_buf, &line);
  terminal_buf_write_null_terminator();
}

void tty_device_init(mb2_tag_framebuffer* framebuffer_tag) {
  mb2_tag_framebuffer_common fb_info = framebuffer_tag->common;
  if (fb_info.framebuffer_type == MB2_FRAMEBUFFER_TYPE_RGB) {
    size_t fb_size = (fb_info.framebuffer_width * fb_info.framebuffer_height) * sizeof(uint32_t);
    mem_range fb_range = {.phys_addr = fb_info.framebuffer_addr, .size = fb_size};
    mem_identity_map_range(&fb_range);
    *device = vbe_ttyd_init(fb_info.framebuffer_addr, fb_info.framebuffer_width, fb_info.framebuffer_height);
  } else {
    *device = vga_ttyd_init(VGA_TEXT_MODE_PHYS_ADDR);
  }
}

void terminal_init(mb2_tag_framebuffer* fb_tag) {
  tty_device_init(fb_tag);
  width = device->get_width();
  height = device->get_height();
  terminal_setcolor(COLOR_LIGHT_GREEN, COLOR_BLACK);
  terminal_clear();
}

uint32_t terminal_get_width() {
  return width;
}

uint32_t terminal_get_height() {
  return height;
}

void terminal_clear() {
  circ_buf_init(&terminal_circ_line_buf, terminal_line_buf, TERMINAL_LINE_LIMIT, sizeof(char*));
  circ_buf_ptr_init(&terminal_buf_wptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  circ_buf_ptr_init(&terminal_line_buf_rptr, terminal_line_buf, terminal_line_buf, TERMINAL_LINE_LIMIT, sizeof(char*));
  terminal_buf_enqueue_line(terminal_buf_wptr.ptr);

  device->clear();
  terminal_set_cursor_pos_xy(0, 0);
  terminal_line = 0;
}

void terminal_delete(size_t n) {
  char* tail_ptr = *((char**) circ_buf_peek_tail(&terminal_circ_line_buf));
  for (size_t i = 0; i < n; i++) {
    if (terminal_buf_wptr.ptr == tail_ptr) {
      circ_buf_dequeue_tail(&terminal_circ_line_buf);
      tail_ptr = *((char**) circ_buf_peek_tail(&terminal_circ_line_buf));
    }
    uint16_t pos = terminal_get_cursor_pos();
    terminal_set_cursor_pos(pos - 1);
    device->put_char(cursor_x, cursor_y, ' ');
    *terminal_buf_wptr.ptr = '\0';
  }
}

void terminal_setcolor(tty_color fg, tty_color bg) {
  device->set_fg(fg);
  device->set_bg(bg);
}

uint16_t terminal_getline() {
  return terminal_line;
}

void terminal_enable_cursor() {
  device->enable_cursor();
}

void terminal_disable_cursor() {
  device->disable_cursor();
}

void terminal_set_cursor_pos(uint64_t pos) {
  terminal_set_cursor_pos_xy(pos % width, pos / width);
}

void terminal_set_cursor_pos_xy(uint32_t x, uint32_t y) {
  cursor_x = x;
  cursor_y = y;
  terminal_buf_wptr.ptr = terminal_cursorxy_to_buf_wptr(x, y);
  device->set_cursor(x, y);
}

uint16_t terminal_get_cursor_pos() {
  return cursor_y * width + cursor_x;
}

void terminal_flush_line(size_t y) {
  uint8_t cursor_x_temp = cursor_x;
  uint8_t cursor_y_temp = cursor_y;
  terminal_set_cursor_pos_xy(0, y);

  struct circ_buf_ptr line_ptr;
  char* line = *((char**) circ_buf_ptr_getoffset(&terminal_line_buf_rptr, y));
  circ_buf_ptr_init(&line_ptr, line, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  while (*line_ptr.ptr != '\0') {
    terminal_putchar_raw(*line_ptr.ptr);
    circ_buf_ptr_increment(&line_ptr);
  }

  terminal_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
}

void terminal_flush() {
  device->clear();

  uint8_t cursor_x_temp = cursor_x;
  uint8_t cursor_y_temp = cursor_y;
  terminal_set_cursor_pos_xy(0, 0);

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < height; i++) {
    line_ptr.ptr = *((char**) circ_buf_ptr_getoffset(&terminal_line_buf_rptr, i));
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_raw(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
    if (i != height - 1) {
      terminal_putchar_raw('\n');
    }
  }

  terminal_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
}

void terminal_scroll_up_force() {
  terminal_line--;
  circ_buf_ptr_decrement(&terminal_line_buf_rptr);
  terminal_flush();
}

void terminal_scroll_down_force() {
  terminal_line++;
  circ_buf_ptr_increment(&terminal_line_buf_rptr);
  terminal_flush();
}

void terminal_scroll_up() {
  if ((char**) terminal_line_buf_rptr.ptr != terminal_line_buf + terminal_circ_line_buf.head) {
    terminal_scroll_up_force();
  }
}

void terminal_scroll_down() {
  if ((char**) circ_buf_ptr_getoffset(&terminal_line_buf_rptr, cursor_y)
      != terminal_line_buf + terminal_circ_line_buf.tail - 1) {
    terminal_scroll_down_force();
  }
}

void terminal_buf_write(char c) {
  if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr.ptr == terminal_buf_read_line(0)) {
    terminal_buf_dequeue_line();
  }
  if (c == '\n') {
    *terminal_buf_wptr.ptr = '\0';
    circ_buf_ptr_increment(&terminal_buf_wptr);
    terminal_buf_enqueue_line(terminal_buf_wptr.ptr);
  } else {
    if (cursor_x == width) {
      circ_buf_ptr_increment(&terminal_buf_wptr);
      terminal_buf_enqueue_line(terminal_buf_wptr.ptr);
    }
    *terminal_buf_wptr.ptr = c;
    circ_buf_ptr_increment(&terminal_buf_wptr);
    terminal_buf_write_null_terminator();
  }
}

static inline void terminal_new_line() {
  cursor_x = 0;
  if (cursor_y == height - 1) {
    terminal_scroll_down_force();
  } else {
    cursor_y++;
  }
}

void terminal_putchar_raw(char c) {
  switch (c) {
    case '\t':
      while ((cursor_x + 1) % TAB_LENGTH != 0) {
        cursor_x++;
      }
      if (cursor_x < width - 1) {
        cursor_x++;
      }
      break;
    case '\b':
      cursor_x -= (cursor_x == 0) ? 0 : 1;
      break;
    case '\n':
      terminal_new_line();
      break;
    default:
      if (cursor_x == width) {
        terminal_new_line();
      }
      device->put_char(cursor_x, cursor_y, c);
      cursor_x++;
  }
  terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_putchar(char c) {
  terminal_buf_write(c);
  terminal_putchar_raw(c);
}

void terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar(data[i]);
  }
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}

void terminal_write_raw(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar_raw(data[i]);
  }
}

void terminal_writestring_raw(const char* data) {
  terminal_write_raw(data, strlen(data));
}

// manual tests:
// buffer limit: TERMINAL_BUFFER_SIZE = 300 --> a, b, c, d, ... until DEQUEUE (check validity)
// line limit: TERMINAL_LINE_LIMIT = 30 --> a, b, c, d, ... until DEQUEUE (check validity + only scroll by 5)

// test writing multi-line commands
// test recalling multi-line commands from history
// test scrolling after writing a multi-line command (2 lines) and then deleting down to 1 line
// test scrolling after recalling multi-line command (4+ lines) then recalling small command (1 line)