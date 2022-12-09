#include <stdio.h>
#include <stdbool.h>

#include <kernel/tty.h>
#include <kernel/kb.h>

void kernel_main() {
	terminal_initialize();
	printf("Hello, Kernel World!\n\n");

	char num_str[100];
	sprintf(num_str, "%x", 12434);
	printf("%s\n", num_str);
	sprintf(num_str, "%d", 12434);
	printf("%s\n", num_str);
	sprintf(num_str, "%o", 12434);
	printf("%s\n", num_str);
	sprintf(num_str, "%b", 12434);
	printf("%s\n\n", num_str);

	sprintf(num_str, "%x\n", 12434);
	printf("%s", num_str);
	sprintf(num_str, "%d\n", 12434);
	printf("%s", num_str);
	sprintf(num_str, "%o\n", 12434);
	printf("%s", num_str);
	sprintf(num_str, "%b\n", 12434);
	printf("%s\n", num_str);

	sprintf(num_str, "%x", 0xDEADBEEF);
	printf("%s\n", num_str);
	sprintf(num_str, "%d", 0xDEADBEEF);
	printf("%s\n", num_str);
	sprintf(num_str, "%o", 0xDEADBEEF);
	printf("%s\n", num_str);
	sprintf(num_str, "%b", 0xDEADBEEF);
	printf("%s\n\n", num_str);

	sprintf(num_str, "%x\n", 0xDEADBEEF);
	printf("%s", num_str);
	sprintf(num_str, "%d\n", 0xDEADBEEF);
	printf("%s", num_str);
	sprintf(num_str, "%o\n", 0xDEADBEEF);
	printf("%s", num_str);
	sprintf(num_str, "%b\n", 0xDEADBEEF);
	printf("%s\n", num_str);

	printf("$ ", num_str);

	bool pressed[128];
	while (1) {
		int scan = keyboard_readscan();
		char ch = keyboard_scan2ch(scan);
		if (keyboard_scan2release(scan)) {
			pressed[scan & 0x7F] = false;
		} else if (!pressed[scan & 0x7F]) {
			printf("%c", ch);
			pressed[scan & 0x7F] = true;
		}
	}
}
