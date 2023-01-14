#include <stdlib.h>

int atoi(const char* nptr) {
  return (int) strtol(nptr, NULL, 10);
}

long int atol(const char* nptr) {
  return strtol(nptr, NULL, 10);
}

long long int atoll(const char* nptr) {
  return strtoll(nptr, NULL, 10);
}