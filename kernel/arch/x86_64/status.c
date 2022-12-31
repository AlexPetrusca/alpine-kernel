#include <kernel/status.h>

char* status_name[] = {"OK", "NOT_FOUND"};

char* status_get_name(status s) {
  return status_name[s];
}
