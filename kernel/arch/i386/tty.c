#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <sys/io.h>
#include <kernel/tty.h>

#define TERMINAL_BUFFER_SIZE 65535
#define TERMINAL_LINE_LIMIT 1000
#define TAB_LENGTH 8

char terminal_buf[TERMINAL_BUFFER_SIZE];
char * terminal_buf_wptr;

char * terminal_line_buf[TERMINAL_LINE_LIMIT];
struct circ_buf terminal_circ_line_buf;
char ** terminal_line_buf_rptr;

uint8_t cursor_x;
uint8_t cursor_y;
uint8_t terminal_color;
uint16_t terminal_line; // todo: maybe remove altogether
uint16_t * terminal_vga_buffer;

// todo: refactor literally EVERYTHING (not just in this file)

// -----
void terminal_writeint_s(int num); // remove me
void terminal_writestring_s(const char * data); // remove me
void terminal_write_s(const char * data, size_t size); // remove me
void terminal_putchar_s(char c); // remove me
void terminal_scroll_down_force(); // remove me
// -----

static inline void terminal_buffer_increment_ptr(char ** ptr) {
	(*ptr)++;
	if (*ptr == terminal_buf + TERMINAL_BUFFER_SIZE) { // reached end, wrap around
		// terminal_writestring_s("-WRAP-");
		*ptr = terminal_buf;
	}
}

static inline void terminal_buffer_decrement_ptr(char ** ptr) {
	if (*ptr == terminal_buf) { // reached end, wrap around
		// terminal_writestring_s("-WRAP-");
		*ptr = terminal_buf + TERMINAL_BUFFER_SIZE;
	}
	(*ptr)--;
}

static inline char * terminal_buffer_add_ptr(char * ptr, size_t n) {
	char * new_ptr = ptr;
	for (size_t i = 0; i < n; i++) {
		terminal_buffer_increment_ptr(&new_ptr);
	}
	return new_ptr;
}

static inline void terminal_line_buffer_increment_ptr(char *** ptr) {
	(*ptr)++;
	if (*ptr == terminal_line_buf + TERMINAL_LINE_LIMIT) { // reached end, wrap around
		*ptr = terminal_line_buf;
	}
}

static inline void terminal_line_buffer_decrement_ptr(char *** ptr) {
	if (*ptr == terminal_line_buf) { // reached end, wrap around
		*ptr = terminal_line_buf + TERMINAL_LINE_LIMIT;
	}
	(*ptr)--;
}

static inline char ** terminal_line_buffer_add_ptr(char ** ptr, size_t n) {
	char ** new_ptr = ptr;
	for (size_t i = 0; i < n; i++) {
		terminal_line_buffer_increment_ptr(&new_ptr);
	}
	return new_ptr;
}

static inline char ** terminal_line_buffer_subtract_ptr(char ** ptr, size_t n) {
	char ** new_ptr = ptr;
	for (size_t i = 0; i < n; i++) {
		terminal_line_buffer_decrement_ptr(&new_ptr);
	}
	return new_ptr;
}

static inline char * terminal_cursorxy_to_buffer_wptr(uint8_t x, uint8_t y) {
	char * line_ptr = *(terminal_line_buffer_add_ptr(terminal_line_buf_rptr, y));
	return terminal_buffer_add_ptr(line_ptr, x);
}

static inline char * terminal_cursor_to_buffer_wptr(uint16_t pos) {
	return terminal_cursorxy_to_buffer_wptr(pos % VGA_WIDTH, pos / VGA_WIDTH);
}

// todo: move this somewhere else
static inline char * terminal_buffer_dequeue_line() {
	// terminal_writestring_s("-DEQUEUE-");
	return *((char **) circ_buf_dequeue(&terminal_circ_line_buf));
}

// todo: move this somewhere else
static inline char * terminal_buffer_read_line(size_t n) {
	return *((char **) circ_buf_peekn(&terminal_circ_line_buf, n));
}

// todo: move this somewhere else
void terminal_buffer_write_null_terminator() {
	if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr == terminal_buffer_read_line(0)) {
		terminal_buffer_dequeue_line();
	}
	*terminal_buf_wptr = '\0';
}

// todo: move this somewhere else
void terminal_buffer_enqueue_line(char * line) {	
	circ_buf_enqueue(&terminal_circ_line_buf, &line);
	// terminal_buffer_write_null_terminator(); // do we need this?
}

void terminal_initialize() {
	terminal_circ_line_buf.buf = (char *) terminal_line_buf;
    terminal_circ_line_buf.capacity = TERMINAL_LINE_LIMIT;
    terminal_circ_line_buf.granularity = sizeof(char *);

	terminal_buf_wptr = terminal_buf;
	terminal_line_buf_rptr = terminal_line_buf;
	terminal_buffer_enqueue_line(terminal_buf_wptr);

	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_vga_buffer = (uint16_t *) VGA_MEMORY;
	terminal_clear();
}

void terminal_clear_row(uint8_t row) {
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		terminal_vga_buffer[row * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
	}
}

void terminal_clear() {
	for (size_t i = 0; i < VGA_HEIGHT; i++) {
		terminal_clear_row(i);
	}
}

void terminal_delete(size_t n) {
	char * tail_ptr = *((char **) circ_buf_peek_tail(&terminal_circ_line_buf));
	for (size_t i = 0; i < n; i++) {
		if (terminal_buf_wptr == tail_ptr) {
			circ_buf_dequeue_tail(&terminal_circ_line_buf);
			tail_ptr = *((char **) circ_buf_peek_tail(&terminal_circ_line_buf));
		}
		uint16_t pos = terminal_get_cursor_pos();
		terminal_set_cursor_pos(pos - 1);
		terminal_vga_buffer[pos - 1] = vga_entry(' ', terminal_color);

		*terminal_buf_wptr = '\0';
		terminal_buffer_decrement_ptr(&terminal_buf_wptr);
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

uint16_t terminal_getline() {
	return terminal_line;
}

void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void terminal_disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void terminal_set_cursor_pos(uint16_t pos) {
	terminal_set_cursor_pos_xy(pos % VGA_WIDTH, pos / VGA_WIDTH);
}

void terminal_set_cursor_pos_xy(uint8_t x, uint8_t y) {
	cursor_x = x;
	cursor_y = y;

	terminal_buf_wptr = terminal_cursorxy_to_buffer_wptr(x, y);

	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t terminal_get_cursor_pos() {
    return cursor_y * VGA_WIDTH + cursor_x;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	terminal_vga_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

char * terminal_getbuffer(char * s, uint16_t offset, uint16_t n) {
	for (size_t i = 0; i < n; i++) {
		s[i] = terminal_vga_buffer[i + offset];
	}
	return s;
}

void terminal_scroll_up_force() {
	memmove(terminal_vga_buffer + VGA_WIDTH, terminal_vga_buffer, VGA_HEIGHT * VGA_WIDTH * sizeof(terminal_vga_buffer));
	terminal_clear_row(0);
	terminal_line--;
	terminal_line_buffer_decrement_ptr(&terminal_line_buf_rptr);
}

void terminal_scroll_down_force() {
	memmove(terminal_vga_buffer, terminal_vga_buffer + VGA_WIDTH, VGA_HEIGHT * VGA_WIDTH * sizeof(terminal_vga_buffer));
	terminal_clear_row(VGA_HEIGHT - 1);
	terminal_line++;
	terminal_line_buffer_increment_ptr(&terminal_line_buf_rptr);
}

void terminal_scroll_up() {
	if (terminal_line_buf_rptr != terminal_line_buf + terminal_circ_line_buf.head) {
		terminal_scroll_up_force();

		uint8_t cursor_x_temp = cursor_x;
		uint8_t cursor_y_temp = cursor_y;
		terminal_set_cursor_pos_xy(0, 0);
		// todo: refactor out
		char * line = *(terminal_line_buf_rptr);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
		}
		terminal_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
	}
}

void terminal_scroll_down() {
	if (terminal_line_buffer_add_ptr(terminal_line_buf_rptr, cursor_y) != terminal_line_buf + terminal_circ_line_buf.tail - 1) {
		terminal_scroll_down_force();

		uint8_t cursor_x_temp = cursor_x;
		uint8_t cursor_y_temp = cursor_y;
		terminal_set_cursor_pos_xy(0, cursor_y);
		// todo: refactor out
		char * line = *terminal_line_buffer_add_ptr(terminal_line_buf_rptr, cursor_y);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
		}
		terminal_set_cursor_pos_xy(cursor_x_temp, cursor_y_temp);
	}
}

// todo: move this somewhere else
// todo: WOW... refactor this shit.
void terminal_buffer_write(char c) {
	if (terminal_circ_line_buf.size > 1 && terminal_buf_wptr == terminal_buffer_read_line(0)) {
		// terminal_writestring_s("-DEQUEUE-");
		terminal_buffer_dequeue_line();
	}
	if (c == '\n') {
		*terminal_buf_wptr = '\0';
		terminal_buffer_increment_ptr(&terminal_buf_wptr);
		terminal_buffer_enqueue_line(terminal_buf_wptr);
	} else {
		*terminal_buf_wptr = c;
		terminal_buffer_increment_ptr(&terminal_buf_wptr);
		terminal_buffer_write_null_terminator();
		if (cursor_x == VGA_WIDTH - 1) {
			terminal_buffer_write_null_terminator();
			terminal_buffer_increment_ptr(&terminal_buf_wptr);
			terminal_buffer_enqueue_line(terminal_buf_wptr);
		}
	}
}

void terminal_putchar(char c) {
	terminal_buffer_write(c);
	switch (c) {
		case '\t':
			while ((cursor_x + 1) % TAB_LENGTH != 0) {
				cursor_x++;
			}
			if (cursor_x < VGA_WIDTH - 1) {
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
			unsigned char uc = c;
			terminal_putentryat(uc, terminal_color, cursor_x, cursor_y);
			cursor_x++;
			if (cursor_x == VGA_WIDTH) {
				cursor_x = 0;
				cursor_y++;
			}
	}
	if (cursor_y >= VGA_HEIGHT) {
		cursor_y--;
		terminal_scroll_down_force();
	}
	terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_write(const char * data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
}

void terminal_writestring(const char * data) {
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
			if (cursor_x < VGA_WIDTH - 1) {
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
			if (cursor_x == VGA_WIDTH) {
				cursor_x = 0;
				cursor_y++;
			}
			unsigned char uc = c;
			terminal_putentryat(uc, terminal_color, cursor_x, cursor_y);
			cursor_x++;
	}
	if (cursor_y >= VGA_HEIGHT) {
		cursor_y--;
		terminal_scroll_down_force();
	}
	terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_write_s(const char * data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar_s(data[i]);
	}
}

void terminal_writestring_s(const char * data) {
	terminal_write_s(data, strlen(data));
}

void terminal_writeint_s(int num) {
	terminal_putchar_s((num / 10) % 10 + '0');
	terminal_putchar_s(num % 10 + '0');
}

void testOverflowByOneNullTerminator() {
	// #define TERMINAL_BUFFER_SIZE 17

	terminal_writestring("az\n\nb\nc\nHello abc");

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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
	for (size_t i = 0; i < 2 * VGA_WIDTH + 3; i++) {
		terminal_putchar('a');
	}
	terminal_putchar('\n');

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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
	for (size_t i = 0; i < 2 * VGA_WIDTH + 3; i++) {
		terminal_putchar('a');
	}

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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
	for (size_t i = 0; i < 2 * VGA_WIDTH + 3; i++) {
		terminal_putchar('a');
	}
	terminal_putchar('\n');

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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

	for (size_t i = 0; i < terminal_circ_line_buf.size; i++) {
		terminal_writestring_s("\n------\n");
		char * line = terminal_buffer_read_line(i);
		while (*line != '\0') {
			terminal_putchar_s(*line);
			terminal_buffer_increment_ptr(&line);
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