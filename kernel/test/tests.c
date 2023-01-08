#include <stdio_tests.h>

#include <kernel/test/tests.h>
#include <sys/dll.h>

void tests_run(__unused int argc, __unused char** argv) {
  test_dll_suite();
  test_sprintf_suite();
  test_memchr_suite();
  test_strchr_suite();
  test_strtok_suite();
  test_strstr_suite();
  test_strcat_suite();
}