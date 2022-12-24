#include <stddef.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <sys/circ_buf_ptr.h>
#include <kernel/tty.h>

#define TERMINAL_BUFFER_SIZE 65535
#define TERMINAL_LINE_LIMIT 1000
#define TAB_LENGTH 8

char terminal_buf[TERMINAL_BUFFER_SIZE];
struct circ_buf_ptr terminal_buf_wptr;

char* terminal_line_buf[TERMINAL_LINE_LIMIT];
struct circ_buf terminal_circ_line_buf;
struct circ_buf_ptr terminal_line_buf_rptr;

uint8_t cursor_x;
uint8_t cursor_y;
uint8_t terminal_color;
uint16_t terminal_line;
tty_device* device;
uint32_t width, height;

// -----
void terminal_writestring_s(const char* data); // remove me
void terminal_write_s(const char* data, size_t size); // remove me
void terminal_putchar_s(char c); // remove me
void terminal_scroll_down_force(); // remove me
// -----

static inline char* terminal_cursorxy_to_buf_wptr(uint8_t x, uint8_t y) {
  // todo: rewrite?
  struct circ_buf_ptr line_ptr;
  char* line = *((char**) circ_buf_ptr_getoffset(&terminal_line_buf_rptr, y));
  circ_buf_ptr_init(&line_ptr, line, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  return circ_buf_ptr_add(&line_ptr, x);
}

static inline char* terminal_cursor_to_buf_wptr(uint16_t pos) {
  return terminal_cursorxy_to_buf_wptr(pos % width, pos / width);
}

// todo: move this somewhere else
static inline char* terminal_buf_dequeue_line() {
  return *((char**) circ_buf_dequeue(&terminal_circ_line_buf));
}

// todo: move this somewhere else
static inline char* terminal_buf_read_line(size_t n) {
  return *((char**) circ_buf_peekn(&terminal_circ_line_buf, n));
}

// todo: move this somewhere else
void terminal_buf_write_null_terminator() {
  if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr.ptr == terminal_buf_read_line(0)) {
    terminal_buf_dequeue_line();
  }
  *terminal_buf_wptr.ptr = '\0';
}

// todo: move this somewhere else
void terminal_buf_enqueue_line(char* line) {
  circ_buf_enqueue(&terminal_circ_line_buf, &line);
   terminal_buf_write_null_terminator(); // do we need this?
}

void terminal_initialize(tty_device* _device) {
  device = _device;
  width = device->get_width();
  height = device->get_height();
  terminal_color = attributes(COLOR_LIGHT_GREEN, COLOR_BLACK);
  terminal_clear();
}

uint32_t terminal_get_width() {
  return device->get_width();
}

uint32_t terminal_get_height() {
  return device->get_height();
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
    device->put_char(cursor_x, cursor_y, ' ', terminal_color);

    *terminal_buf_wptr.ptr = '\0';
    circ_buf_ptr_decrement(&terminal_buf_wptr);
  }
}

void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}

uint16_t terminal_getline() {
  return terminal_line;
}

void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
  device->enable_cursor(cursor_start, cursor_end);
}

void terminal_disable_cursor() {
  device->disable_cursor();
}

void terminal_set_cursor_pos(uint16_t pos) {
  terminal_set_cursor_pos_xy(pos % width, pos / width);
}

void terminal_set_cursor_pos_xy(uint8_t x, uint8_t y) {
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
    terminal_putchar_s(*line_ptr.ptr);
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
    uint32_t written_chars = 0;
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
      written_chars++;
    }
    while (written_chars < width) {
      terminal_putchar_s(' ');
      written_chars++;
    }
    if (i != height - 1) {
      terminal_putchar_s('\n');
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

// todo: move this somewhere else
void terminal_buf_write(char c) {
  if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr.ptr == terminal_buf_read_line(0)) {
    terminal_buf_dequeue_line();
  }
  if (c == '\n') {
    *terminal_buf_wptr.ptr = '\0';
    circ_buf_ptr_increment(&terminal_buf_wptr);
    terminal_buf_enqueue_line(terminal_buf_wptr.ptr);
  } else {
    *terminal_buf_wptr.ptr = c;
    circ_buf_ptr_increment(&terminal_buf_wptr);
    terminal_buf_write_null_terminator();
    if (cursor_x == width - 1) {
      terminal_buf_write_null_terminator();
      circ_buf_ptr_increment(&terminal_buf_wptr);
      terminal_buf_enqueue_line(terminal_buf_wptr.ptr);
    }
  }
}

void terminal_putchar(char c) {
  terminal_buf_write(c);
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
      cursor_y++;
      cursor_x = 0;
      break;
    default:
      device->put_char(cursor_x, cursor_y, c, terminal_color);
      cursor_x++;
      if (cursor_x == width) {
        cursor_x = 0;
        cursor_y++;
      }
  }
  if (cursor_y >= height) {
    cursor_y--;
    terminal_scroll_down_force();
  }
  terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar(data[i]);
  }
}

void terminal_writestring(const char* data) {
  terminal_write(data, strlen(data));
}

// -----------------------------------------------------------------------
// REMOVE THE FOLLOWING
// -----------------------------------------------------------------------

// manual tests:
// buffer limit: TERMINAL_BUFFER_SIZE = 300 --> a, b, c, d, ... until DEQUEUE (check validity)
// line limit: TERMINAL_LINE_LIMIT = 30 --> a, b, c, d, ... until DEQUEUE (check validity + only scroll by 5)

// test writing multi-line commands
// test recalling multi-line commands from history
// test scrolling after writing a multi-line command (2 lines) and then deleting down to 1 line
// test scrolling after recalling multi-line command (4+ lines) then recalling small command (1 line)

void terminal_putchar_s(char c) {
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
      cursor_y++;
      cursor_x = 0;
      break;
    default:
      if (cursor_x == width) {
        cursor_x = 0;
        cursor_y++;
      }
      device->put_char(cursor_x, cursor_y, c, terminal_color);
      cursor_x++;
  }
  if (cursor_y >= height) {
    cursor_y--;
    terminal_scroll_down_force();
  }
  terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_write_s(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar_s(data[i]);
  }
}

void terminal_writestring_s(const char* data) {
  terminal_write_s(data, strlen(data));
}

void testOverflowByOneNullTerminator() {
  // #define TERMINAL_BUFFER_SIZE 17

  terminal_writestring("az\n\nb\nc\nHello abc");

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 4
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testOverflowAndStringWrapAround() {
  // #define TERMINAL_BUFFER_SIZE 17

  terminal_writestring("az\n\nb\nc\nHello abcdef");

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 3
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testOverflowAndStringWrapAroundAndLineLimit() {
  // #define TERMINAL_BUFFER_SIZE 17
  // #define TERMINAL_LINE_LIMIT 3

  terminal_writestring("az\n\nb\nc\nHello\nabcdef");

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 3
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testImplicitNewLine() {
  // #define TERMINAL_BUFFER_SIZE 167

  // 81 + 81 + 4 + 1 = 167
  for (size_t i = 0; i < 2 * width + 3; i++) {
    terminal_putchar('a');
  }
  terminal_putchar('\n');

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 4
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testImplicitNewLineAndPerfectFitCondition() {
  // #define TERMINAL_BUFFER_SIZE 166

  // 81 + 81 + 4 = 166
  for (size_t i = 0; i < 2 * width + 3; i++) {
    terminal_putchar('a');
  }

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 3
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testImplicitNewLineAndOverflow() {
  // #define TERMINAL_BUFFER_SIZE 165

  // 81 + 81 + 4 = 166
  for (size_t i = 0; i < 2 * width + 3; i++) {
    terminal_putchar('a');
  }
  terminal_putchar('\n');

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 3
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void testCursorIntoTerminalBuffer() {
  terminal_writestring("Hello \t World!\n"); // Hello     World!
  terminal_writestring("Hello \b World!\n"); // Hello World!

  struct circ_buf_ptr line_ptr;
  circ_buf_ptr_init(&line_ptr, terminal_buf, terminal_buf, TERMINAL_BUFFER_SIZE, sizeof(char));
  for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
    line_ptr.ptr = terminal_buf_read_line(i);
    terminal_writestring_s("\n------\n");
    while (*line_ptr.ptr != '\0') {
      terminal_putchar_s(*line_ptr.ptr);
      circ_buf_ptr_increment(&line_ptr);
    }
  }
  terminal_writestring_s("\n------\n");

  terminal_putchar_s('\n');
  terminal_writestring_s("Size: "); // 3
  terminal_putchar_s('0' + terminal_circ_line_buf.size);
}

void test() {
  // testOverflowByOneNullTerminator(); // pass
  // testOverflowAndStringWrapAround(); // pass
  // testOverflowAndStringWrapAroundAndLineLimit(); // pass
  // testImplicitNewLine(); // pass
  // testImplicitNewLineAndPerfectFitCondition(); // pass
  // testImplicitNewLineAndOverflow(); // pass
  testCursorIntoTerminalBuffer();
}