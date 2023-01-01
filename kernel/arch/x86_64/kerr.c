#include <kernel/kerr.h>

kerr_code_t kerr_code = 0;
char* kerr_message = NULL;
char* kerr_error_name[] = {"OK", "MISSING_INFO", "NOT_FOUND", "NOT_INITIALIZED", "LIMIT_EXCEEDED", "UNKNOWN"};

kerr_code_t kerr_get_code() {
  return kerr_code;
}

void kerr_raise(kerr_code_t code, char* message) {
  if (kerr_code != OK) {
    panic("Double kernel error raised: %s\n", message);
  }
  kerr_code = code;
  kerr_message = message;
}

void kerr_clear() {
  kerr_code = OK;
  kerr_message = NULL;
}

char* kerr_get_name() {
  return kerr_error_name[kerr_code];
}

char* kerr_get_message() {
  return kerr_message;
}