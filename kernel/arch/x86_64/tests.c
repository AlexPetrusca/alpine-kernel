#include <kernel/tests.h>
#include "stdio_tests.h"
#include <kernel/queue.h>

void tests_run() {
  test_dequeue_suite();
  test_sprintf_suite();
}