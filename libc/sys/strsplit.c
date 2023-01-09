#include <stdlib.h>
#include <string.h>

#include <sys/strsplit.h>

vector* strsplit(char* str, const char* delim) {
  vector* split = vector_new();
  char* token = strtok(str, delim);
  while (token != NULL) {
    vector_add(split, token);
    token = strtok(NULL, delim);
  }
  return split;
}