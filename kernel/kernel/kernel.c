#include <stdio.h>

#include <sys/circ_buf.h>
#include <kernel/tty.h>
#include <kernel/sh.h>

char buffer[4096 * 4];
struct circ_buf cb_struct;

void test_circ_buf() {
	circ_buf_t cb = &cb_struct;
	cb->buf = buffer;
	cb->capacity = 4;
	cb->granularity = 4096;
	
	char * s = "abc";
	circ_buf_push(cb, s);
	s = "def";
	circ_buf_push(cb, s);
	s = "ghi";
	circ_buf_push(cb, s);
	s = "jkl";
	circ_buf_push(cb, s);
	s = "mno";
	circ_buf_push(cb, s);
	s = "pqr";
	circ_buf_push(cb, s);

	printf("buffer (peekn_head): \n");
	for (int i = 0; i < cb->size; i++) {
		char * s = circ_buf_peekn_head(cb, i);
		printf("    %d: %s\n", i , s);
	}
	printf("buffer (peekn_tail): \n");
	for (int i = 0; i < cb->size; i++) {
		char * s = circ_buf_peekn_tail(cb, i);
		printf("    %d: %s\n", i , s);
	}
	printf("buffer (pop): \n");
	for (int i = 0; !circ_buf_empty(cb); i++) {
		char * s = circ_buf_pop(cb);
		printf("    %d: %s\n", i , s);
	}
	printf("head: %d\n", cb->head);
	printf("tail: %d\n", cb->tail);
	printf("size: %d\n", cb->size);
	printf("capacity: %d\n", cb->capacity);
}

void kernel_main() {
	terminal_initialize();
	shell_start();
	// test_circ_buf();
}
