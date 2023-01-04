#include <stdio.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <kernel/device/kb.h>
#include <kernel/tty/tty.h>
#include <kernel/shell/sh.h>
#include <kernel/tty/vga_ttyd.h>
#include <kernel/cpu/cpu.h>
#include <stdio_tests.h>

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
  sh_command commands[] = {{"cpu",  print_cpu_info},
                           {"test", printf_tests},
                           {"", NULL}};
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
