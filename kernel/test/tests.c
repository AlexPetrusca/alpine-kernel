#include <kernel/test/test.h>
#include <kernel/test/tests.h>

void tests_run(__unused int argc, __unused char** argv) {
  RUN_SUITE(dll);
  RUN_SUITE(memchr);
  RUN_SUITE(sprintf);
  RUN_SUITE(strchr);
  RUN_SUITE(strtok);
  RUN_SUITE(strstr);
  RUN_SUITE(strcat);
  RUN_SUITE(strdup);
  RUN_SUITE(strsplit);
  RUN_SUITE(strtrim);
  RUN_SUITE(heap);
  RUN_SUITE(vector);
}