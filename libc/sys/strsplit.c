#include <stdlib.h>
#include <string.h>

#include <sys/strsplit.h>

strsplit_t* strsplit(char* str, const char* delim) {
  strsplit_t* split = vector_new();
  char* dup = strdup(str);
  char* token = strtok(dup, delim);
  while (token != NULL) {
    vector_add(split, token);
    token = strtok(NULL, delim);
  }
  return split;
}

void strsplit_free(strsplit_t* strsplit) {
  free(vector_get(strsplit, 0));
  vector_free(strsplit);
}