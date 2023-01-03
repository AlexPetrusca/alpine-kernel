#include <kernel/tests.h>
#include "stdio_tests.h"
#include <kernel/dll.h>

void tests_run() {
  test_dll_suite();
  test_sprintf_suite();
}