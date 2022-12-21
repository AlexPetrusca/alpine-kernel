#include <stdio.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <kernel/kb.h>
#include <kernel/tty.h>
#include <kernel/sh.h>
#include <kernel/vga.h>
#include <kernel/cpu.h>

void test_circ_buf() {
  char buffer[4096 * 4];
  struct circ_buf cb_struct;
  circ_buf_t cb = &cb_struct;
  cb->buf = buffer;
  cb->capacity = 4;
  cb->granularity = 4096;

  char* s = "abc";
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
  for (size_t i = 0; i < cb->size; i++) {
    char* s = circ_buf_peekn_head(cb, i);
    printf("    %d: %s\n", i, s);
  }
  printf("buffer (peekn_tail): \n");
  for (size_t i = 0; i < cb->size; i++) {
    char* s = circ_buf_peekn_tail(cb, i);
    printf("    %d: %s\n", i, s);
  }
  printf("buffer (pop): \n");
  for (int i = 0; !circ_buf_empty(cb); i++) {
    char* s = circ_buf_pop(cb);
    printf("    %d: %s\n", i, s);
  }
  printf("head: %d\n", cb->head);
  printf("tail: %d\n", cb->tail);
  printf("size: %d\n", cb->size);
  printf("capacity: %d\n", cb->capacity);
}

void test_memchr() {
  char* s1 = "";
  char* s2 = "abcdefabcdef";
  char* s3 = "11111111111111111111";

  printf("Testing memchr():\nTest1...");
  if (memchr(s1, 'x', 0) == NULL)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest2...");
  if (memchr(s2, 'y', 0) == NULL)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest3...");
  if ((char*) memchr(s2, 'a', 1) - s2 == 0)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest4...");
  if (memchr(s2, 'd', 2) == NULL)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest5...");
  if ((char*) memchr(s2, 'd', 12) - s2 == 3)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest6...");
  if ((char*) memchr(s2, 'f', 12) - s2 == 5)
    printf("passed.");
  else printf("FAILED.");
  printf("\nTest7...");
  if ((char*) memchr(s3, '1', 20) - s3 == 0)
    printf("passed.");
  else printf("FAILED.");
  putchar('\n');
}

void test_memrchr() {
  printf("Testing memrchr():\n");
  const char str[] = "http://www.tutorialspoint.com";
  const char ch = '.';

  char* ret = memrchr(str, ch, strlen(str));
  printf("String after |%c| is - |%s|\n", ch, ret);
}

void test_strchr() {
  printf("Testing strchr():\n");
  const char str[] = "http://www.tutorialspoint.com";
  const char ch = '.';

  char* ret = strchr(str, ch);
  printf("String after |%c| is - |%s|\n", ch, ret);
}

void test_strrchr() {
  printf("Testing strrchr():\n");
  const char str[] = "http://www.tutorialspoint.com";
  const char ch = '.';

  char* ret = strrchr(str, ch);
  printf("String after |%c| is - |%s|\n", ch, ret);
}

void test_strtok() {
  printf("Testing strtok():\n");
  char str[80] = "This is - www.tutorialspoint.com - website";
  const char s[2] = "-";
  char* token;

  token = strtok(str, s);
  while (token != NULL) {
    printf("%s\n", token);
    token = strtok(NULL, s);
  }
}

void test_strtok_r() {
  printf("Testing strtok_r():\n");
  char str[80] = "This is - www.tutorialspoint.com - website";
  const char s[2] = "-";
  char* rest;
  char* token;

  token = strtok_r(str, s, &rest);
  while (token != NULL) {
    printf("%s\n", token);
    token = strtok_r(NULL, s, &rest);
  }
}

void test_strstr() {
  printf("Testing strstr():\n");
  const char haystack[20] = "TutorialsPoint";
  const char needle[10] = "Point";
  char* ret;

  ret = strstr(haystack, needle);
  printf("The substring is: %s\n", ret);
}

void test_strcat() {
  printf("Testing strcat():\n");
  char src[50], dest[50];
  strcpy(src, "This is source");
  strcpy(dest, "This is destination");

  strcat(dest, src);
  printf("Final destination string : |%s|\n", dest);
}

void test_strncat() {
  printf("Testing strncat():\n");
  char src[50], dest[50];
  strcpy(src, "This is source");
  strcpy(dest, "This is destination");

  strncat(dest, src, 15);
  printf("Final destination string : |%s|\n", dest);
}

void test_kb() {
  while (1) {
    int scan = keyboard_readscan();
    char ch = keyboard_scan2ch(scan);
    if (keyboard_scan2release(scan)) {
      keyboard_set_released(ch);
    } else if (keyboard_is_released(ch)) {
      keyboard_set_pressed(ch);
      printf("{scan: %d, ch: %d, display: %c}\n", scan & 0x7F, ch, ch);
    }
  }
}

void kernel_main() {
  terminal_initialize(&vga_tty_device);
  sh_command commands[] = {{"cpu", print_cpu_info}, {"", NULL}};
  shell_start(commands);
  // test(); // todo: remove me

  // test_kb();
  // test_circ_buf();
  // test_memchr();
  // test_memrchr();
  // test_strchr();
  // test_strrchr();
  // test_strtok();
  // test_strtok_r();
  // test_strstr();
  // test_strcat();
  // test_strncat();
}
